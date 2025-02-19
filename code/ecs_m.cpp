#include <algorithm>
#include <iomanip>
#include "ecs_m.h"
#include "exceptions.h"
#include <optional>

Ecs_m::Ecs_m() : systems({}) {}
Ecs_m::Ecs_m(V<System*> systems) : systems(systems), fl(1.0/60.0) {
    for(int i = 0; i < systems.size(); i++){
        if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(systems[i])){
            intersections.insert({systems[i], {}});
            for(auto it = sys->event_callbacks.begin(); it != sys->event_callbacks.end(); ++it){
                if(event_subscribers.find(it->first) == event_subscribers.end()){
                    event_subscribers.insert({it->first, {}});
                }
                event_subscribers.at(it->first).push_back(systems[i]);
            }
        }
        if(Entity_for_all_system* sys = dynamic_cast<Entity_for_all_system*>(systems[i])){
            intersections.insert({systems[i], {}});
        }
        if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(systems[i])){
            intersections.insert({systems[i], {}});
        }

        for(Typ typ : systems[i]->typs){
            comps.insert({typ, Component_list(typ)});
        }
    }
    for(int i = 0; i < AMOUNT_OF_FLAGS; i++){
        flag_to_entities.insert({1 << i, {}});
    }
}

void Ecs_m::add(Component* el, Id entity_id, Typ typ){
    if(comps.find(typ) == comps.end()){
        comps.insert({typ, Component_list(typ)});
    }
    el->set_entity_id(entity_id);
    comps.at(typ).add(static_cast<void*>(el)); 
    add_queue.push_front(el->comp_id);
    
    comp_id_to_typ.insert({el->comp_id, typ});
    comp_id_to_entity_id.insert({el->comp_id, entity_id});
    if(entity_id_to_comp_id.find(entity_id) == entity_id_to_comp_id.end()){
        entity_id_to_comp_id.insert({entity_id, {el->comp_id}});
        entity_id_to_typs.insert({entity_id, {typ}});
    } else {
        entity_id_to_comp_id.at(entity_id).push_back(el->comp_id);
        entity_id_to_typs.at(entity_id).push_back(typ);
    }
}

void Ecs_m::remove(Id comp_id){
    if(comp_id_to_entity_id.find(comp_id) != comp_id_to_entity_id.end()){
        delete_queue.push_front(comp_id);
    }
}

void Ecs_m::add_flag(flag flag, Id entity_id) {
    int64_t indexer = 1;
    for(int i = 0; i < 64; i++){
        int64_t flag_value = (indexer << i) & flag; 
        if(flag_value){
            auto ids = flag_to_entities.find(flag_value);
            if(ids != flag_to_entities.end()){
                ids->second.push_back(entity_id);
            } else {
                flag_to_entities.insert({flag_value, {entity_id}});
            }
        }
    }
}

void Ecs_m::remove_flag(flag flag, Id entity_id){
    int64_t indexer = 1;
    for(int i = 0; i < 64; i++){
        int64_t flag_value = (indexer << i) & flag; 
        if(flag_value){
            auto ids = flag_to_entities.find(flag_value);
            if(ids != flag_to_entities.end()){
                ids->second.erase(std::find(ids->second.begin(), ids->second.end(), entity_id));
            } 
        }
    }
}

// Remove first component of Type typ from entity:
void Ecs_m::remove_typ(Id entity_id, Typ typ){
    for(Id id : entity_id_to_comp_id.at(entity_id)){
        if(comp_id_to_typ.at(id) == typ){
            remove(id);
            return;
        }
    }
    std::cout << "\033[31mWARNING: TRIED TO REMOVE TYPE " 
                << comps_metadata.at(typ).name 
                << " FROM ENTITY " << std::to_string(entity_id) 
                << " BUT THERE WAS NONE!\033[0m" << std::endl;
}

void Ecs_m::add_all_in_queues() {
    while(add_queue.size() > 0){
        Id comp_id = add_queue.front();

        comps.at(comp_id_to_typ.at(comp_id)).add_all_in_add_queue();
        add_intersections(comp_id_to_entity_id.at(comp_id), comp_id_to_typ.at(comp_id));

        add_queue.pop_front();
    }
}

void Ecs_m::remove_all_in_queues(){
    while(delete_queue.size() > 0){
        Id comp_id = delete_queue.front();
        
        Typ typ = comp_id_to_typ.at(comp_id);
        Id entity_id = comps.at(typ).entity_of_comp(comp_id);

        comps.at(typ).remove(comps.at(typ).index_of_comp(comp_id));            
        comp_id_to_typ.erase(comp_id);
        comp_id_to_entity_id.erase(comp_id);
        auto comp_ids = &entity_id_to_comp_id.at(entity_id);
        comp_ids->erase(std::find(comp_ids->begin(), comp_ids->end(), comp_id));
        auto typs = &entity_id_to_typs.at(entity_id);
        typs->erase(std::find(typs->begin(), typs->end(), typ));

        remove_intersections(entity_id);  

        delete_queue.pop_front();
    }
}

void Ecs_m::add_intersections(Id entity_id, Typ type_added) {
    if(count(entity_id_to_typs.at(entity_id), type_added) > 1){return;} //Check if this type of element has already been added before

    for(auto it = intersections.begin(); it != intersections.end(); ++it){
        if(!exists(it->first->typs, type_added)){goto dont_add;} // Is the type we are adding even relevant for the system in question? 

        for(Typ typ : it->first->typs){
            if(comps.find(typ) == comps.end()){
                goto dont_add;
            }
            if(!exists(entity_id_to_typs.at(entity_id), typ)){
                goto dont_add;
            }
        }
        if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(it->first)){
            for(flag flag : sys->flags){
                if( flag_to_entities.find(flag) == flag_to_entities.end()){
                    goto dont_add;
                }
                if(!exists(flag_to_entities.at(flag), entity_id)){
                    goto dont_add;
                }
            }
        }

        it->second.insert(entity_id);
        first_frame_systems.insert({it->first, entity_id});
        
        if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(it->first)){
            System_data* data = sys->initialize_data();
            if(data != nullptr){
                systems_data.insert({{it->first, entity_id}, data});
            }
        }

        dont_add:
        continue;
    }
}
void Ecs_m::remove_intersections(Id entity_id){
    for(auto it = intersections.begin(); it != intersections.end(); ++it){
        for(Typ typ : it->first->typs){
            if(!exists(entity_id_to_typs.at(entity_id), typ)){
                goto remove;
            }
        }
        if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(it->first)){
            for(flag flag : sys->flags){
                if(!exists(flag_to_entities.at(flag), entity_id)){
                    goto remove;
                }
            }
        }
        continue;

        remove:
        it->second.erase(entity_id);
        if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(it->first)){
            if(auto data_it = systems_data.find({it->first, entity_id}); data_it != systems_data.end()){
                sys->destroy_data(data_it->second);
            }
        }

        continue;
    }
}

void Ecs_m::update() {

    // Refresh any layers that need it:
    for(auto& [layer_name, layer] : graphic_layers){
        set_active_graphic_layer(layer_name);
        if(graphic_layer_refreshes.at(layer_name)){
            ClearBackground(BLANK);
        }
    }

    // Run first frame callbacks;
    for(System_instance sys_inst : first_frame_systems){
        processing_system = sys_inst.sys;
        processing_id = sys_inst.entity_id;
        set_active_graphic_layer(sys_inst.sys->graphic_layer);

        if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(sys_inst.sys)){
            sys->first_frame(sys_inst.entity_id);
        }
        if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(sys_inst.sys)){
            sys->first_frame(sys_inst.entity_id);
        }
    }
    first_frame_systems={};

    // Run event callbacks:
    for(auto it = events.begin(); it != events.end();){
        it->frames -= 1;
        if(it->frames <= 0){
            for(System* sub_sys : event_subscribers.at(it->event)){
                set_active_graphic_layer(sub_sys->graphic_layer);
                if(General_system* sys = dynamic_cast<General_system*>(sub_sys)){
                    sys->event_callbacks.at(it->event)(it->data);
                } 
                else if(General_system_with_data* sys = dynamic_cast<General_system_with_data*>(sub_sys)){
                    sys->event_callbacks.at(it->event)(it->data);
                }
                else {
                    for(Id entity_id : intersections.at(sub_sys)){
                        if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(sub_sys)){
                            sys->event_callbacks.at(it->event)(entity_id, it->data);
                        }
                        if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(sub_sys)){
                            sys->event_callbacks.at(it->event)(entity_id, it->data);
                        }
                    }
                }  
            }
            it = events.erase(it);
        } else {
            ++it;
        }
    }

    // Run timeout callbacks;
    for(auto it = next_frame_calls.begin(); it != next_frame_calls.end();){
        it->frames -= 1;
        if(it->frames <= 0){
            if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(it->system)){
                if(exists(intersections.at(it->system), it->id)){
                    set_active_graphic_layer(it->system->graphic_layer);
                    it->call();
                }
            }
            else if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(it->system)){
                if(exists(intersections.at(it->system), it->id)){
                    set_active_graphic_layer(it->system->graphic_layer);
                    it->call();
                }
            }
            else if(Component_individual_system* sys = dynamic_cast<Component_individual_system*>(it->system)){
                if(comp_id_to_entity_id.find(it->id) != comp_id_to_entity_id.end()){
                    set_active_graphic_layer(it->system->graphic_layer);
                    it->call();
                }
            }
            else if(General_system* sys = dynamic_cast<General_system*>(it->system)){
                set_active_graphic_layer(it->system->graphic_layer);
                it->call();
            }
            else if(General_system_with_data* sys = dynamic_cast<General_system_with_data*>(it->system)){
                set_active_graphic_layer(it->system->graphic_layer);
                it->call();
            }
            it = next_frame_calls.erase(it);
        } else {
            ++it;
        }
    }

    // Run update callbacks;
    for(int i = 0; i < systems.size(); i++){
        processing_system = systems[i];
        set_active_graphic_layer(systems[i]->graphic_layer);

        if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(systems[i])){
            for(Id entity_id : intersections.at(systems[i])){
                processing_id = entity_id;
                sys->update(entity_id);
            }   
            continue;
        }
        else if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(systems[i])){
            for(Id entity_id : intersections.at(systems[i])){
                processing_id = entity_id;
                sys->update(entity_id, systems_data.at({systems[i], entity_id}));
            }   
            continue;
        }
        else if(Entity_for_all_system* sys = dynamic_cast<Entity_for_all_system*>(systems[i])){
            sys->update(intersections.at(systems[i]));
            continue;
        }
        else if(Component_individual_system* sys = dynamic_cast<Component_individual_system*>(systems[i])){
            for(int j = 0; j < comps.at(sys->typs.at(0)).size; j++){
                Component* comp = comps.at(sys->typs.at(0)).get(j);
                processing_id = comp->comp_id;
                sys->update(comp);
            }                
        }
        else if(Component_for_all_system* sys = dynamic_cast<Component_for_all_system*>(systems[i])){
            Typ ty = sys->typs.at(0);
            auto lst = comps.at(sys->typs.at(0));
            V<Component*> els(lst.size);
            for(int j = 0; j < lst.size; j++){
                els[j] = lst.get(j);
            }  
            sys->update(els);
        }
        else if(General_system* sys = dynamic_cast<General_system*>(systems[i])){
            sys->update();
        }
        if(General_system_with_data* sys = dynamic_cast<General_system_with_data*>(systems[i])){
            sys->update(systems_data.at({systems[i], 0}));
        }
    }
    EndTextureMode();

    add_all_in_queues();
    remove_all_in_queues();
}


void Ecs_m::set_time_scale(float scale){
    fl = (1.0/60.0)*scale;
    std::cout << "fl is now: " << fl << std::endl;
}

Component* Ecs_m::get_from_entity(Id entity_id, Typ typ){
    return static_cast<Component*>(comps.at(typ).get_from_entity(entity_id)); 
}

void* Ecs_m::get_from_comp_with_typ(Id component_id, Typ typ){
    return comps.at(typ).get_from_comp(component_id);
}

Id Ecs_m::get_entity_id(Id comp_id){
    auto res = comp_id_to_entity_id.find(comp_id);
    if(res != comp_id_to_entity_id.end()){
        return res->second;
    }
    throw NullPtrException("Component with id " + std::to_string(comp_id) + " has no corresponding entity");
}

V<Id> Ecs_m::get_all_entities_with_flag(flag flag) {
    return flag_to_entities.at(flag);
}

bool Ecs_m::has_type(Id entity_id, Typ typ){
    return exists(entity_id_to_typs.at(entity_id), typ);
}
bool Ecs_m::contains_sibling_type(V<Id> comp_ids, Typ typ){
    for(Id comp_id : comp_ids){
        if(has_type(comp_id_to_entity_id.find(comp_id)->second, typ)){
            return true;
        }
    }
    return false;
}
std::optional<Id> Ecs_m::get_first_sibling_of_type(V<Id> comp_ids, Typ typ){
    for(Id comp_id : comp_ids){
        auto entity_id = comp_id_to_entity_id.find(comp_id);
        if(entity_id != comp_id_to_entity_id.end() && has_type(entity_id->second, typ)){
            return static_cast<Component*>(comps.at(typ).get_from_entity(entity_id->second))->comp_id;
        }
    }
    return {};
}
bool Ecs_m::has_flag(Id entity_id, flag flag) {
    return exists(flag_to_entities.at(flag), entity_id);
}


// SPECIAL FEATURES:
void Ecs_m::timeout(int frames, std::function<void(void)> call) {
    next_frame_calls.push_back({
        .call = call, 
        .id = processing_id, 
        .system = processing_system, 
        .frames = frames
    });   
}
void Ecs_m::timeout_time(float time, std::function<void(void)> call){
    timeout(static_cast<int>(round(time/fl)), call);
}

void Ecs_m::emit_event(EVENT event, Event_data data, int frames){
    if(event_subscribers.find(event) == event_subscribers.end()){
        event_subscribers.insert({event, {}});
    }
    events.push_back({event, data, frames});
}


// DEBUG:
void Ecs_m::print_table() {
    // Gather all unique component types
    const int width = 10;

    std::set<Typ> all_types;
    for (const auto& [entity_id, types] : entity_id_to_typs) {
        all_types.insert(types.begin(), types.end());
    }

    // Print the header row
    std::cout << std::setw(width) << "Entity ID";
    for (const Typ& typ : all_types) {
        std::cout << std::setw(width) << comps_metadata.at(typ).shorthand;
    }
    std::cout << "\n";

    // Print a separator
    std::cout << std::string(width, '-') << "+";
    for (size_t i = 0; i < all_types.size(); ++i) {
        std::cout << std::string(width, '-') << "+";
    }
    std::cout << "\n";

    // Print each entity's row
    for (const auto& [entity_id, comp_ids] : entity_id_to_comp_id) {
        std::cout << std::setw(width);
        auto entity_name = named_entities.find(entity_id);
        if(entity_name != named_entities.end()){
            std::string s = entity_name->second + " | " + std::to_string(entity_id);
            std::cout << s;
        } else {
            std::cout << entity_id;
        }

        for (const Typ& typ : all_types) {
            // Find all component IDs of this type for the current entity
            std::vector<Id> matching_ids;
            for (const Id& comp_id : comp_ids) {
                if (comp_id_to_typ.at(comp_id) == typ) {
                    matching_ids.push_back(comp_id);
                }
            }

            // Format the cell with the list of component IDs
            std::string cell_content = "[";
            for (size_t i = 0; i < matching_ids.size(); ++i) {
                cell_content += std::to_string(matching_ids[i]);
                if (i < matching_ids.size() - 1) {
                    cell_content += ", ";
                }
            }
            cell_content += "]";

            std::cout << std::setw(width) << cell_content;
        }
        std::cout << "\n\n";
    }
}