#include "ecs_manager.h"
#include <sstream>
#include "exceptions.h"

void ECS_manager::remove_component(std::type_index typ, int16_t comp_id){
    component_maps[typ]->remove(comp_id);
}
void ECS_manager::remove_components(std::type_index typ, int16_t entity_id){
    component_maps[typ]->remove_all(entity_id);
}

ECS_manager::ECS_manager(
    std::unordered_map<std::type_index, BaseComponentMap*> typ_map, 
    std::unordered_map<std::type_index, std::string>& typ_index_to_string,
    std::vector<System*> systems
    ) : 
    component_maps(typ_map), 
    systems(systems),
    typ_index_to_string(typ_index_to_string)
    {}

void ECS_manager::add_component(int16_t entity_id, Component* comp){
    if( !comp->allows_multiplicity && 
        component_maps[typeid(*comp)]->contains_entity_id(entity_id)){
        throw IllegalMultipleComponents(comp->name, entity_id);
    }

    component_maps[typeid(*comp)]->add(comp, entity_id);
}

bool ECS_manager::has_component(int16_t entity_id, std::type_index typ){
    return component_maps[typ]->get(entity_id) != nullptr;
}

Component* ECS_manager::get_component(int16_t comp_id, std::type_index typ){
    return component_maps[typ]->get(comp_id);
}
std::vector<Component*> ECS_manager::get_components(int16_t entity_id, std::type_index typ){
    return component_maps[typ]->get_all(entity_id);
}

bool ECS_manager::singleton_exists(std::type_index typ){
    return component_maps[typ]->id_map.size() != 0;
}
Component* ECS_manager::get_singleton(std::type_index typ) {
    if(component_maps[typ]->id_map.size() == 0){
        throw NullPtrException("Trying to get singleton of typ " + get_str(typ) + ", which hasn't yet been added!");
    }
    return component_maps[typ]->get_first();
}

bool ECS_manager::is_typ(int16_t comp_id, std::type_index typ) {
    return component_maps[typ]->contains_comp_id(comp_id);
}

int16_t ECS_manager::get_entity(int16_t component_id, std::type_index typ){
    return component_maps[typ]->id_map[component_id];
}

static bool print_calls = false;

void ECS_manager::update() {
    for(auto sys : systems){
        if (Entity_System_Individual* esys = dynamic_cast<Entity_System_Individual*>(sys)){
            // Iterate through only those entities that have all components (take intersection of ids)
            auto ids = component_maps[esys->typs[0]]->get_all_ids();
            for(int i = 1; i < esys->typs.size(); i++){
                auto ids_i = component_maps[esys->typs[i]]->get_all_ids();
                std::set<int16_t> intersection = {};
                std::set_intersection(ids.begin(), ids.end(), ids_i.begin(), ids_i.end(), std::inserter(intersection, intersection.begin()));
                ids = intersection;
            }

            for(int16_t id : ids){
                std::vector<std::vector<Component*>> comps(esys->typs.size());
                for(int j=0; j<esys->typs.size(); j++){
                    comps[j]=component_maps[esys->typs[j]]->get_all(id);
                }
                if(print_calls){
                    std::cout << "Calling indvidual-system " << sys->name << " for id " << id << " with: ";
                    for(int j=0; j<esys->typs.size(); j++){
                        std::cout << comps[j][0]->name << "[" << comps[j].size() << "], ";
                    }
                    std::cout << std::endl;
                }
                esys->update(id, this, comps);
            }

        } 
        else if(Entity_System_All* esys = dynamic_cast<Entity_System_All*>(sys)){
            // Iterate through only those entities that have all components (take intersection of ids)
            auto ids = component_maps[esys->typs[0]]->get_all_ids();
            for(int i = 1; i < esys->typs.size(); i++){
                auto ids_i = component_maps[esys->typs[i]]->get_all_ids();
                std::set<int16_t> intersection = {};
                std::set_intersection(ids.begin(), ids.end(), ids_i.begin(), ids_i.end(), std::inserter(intersection, intersection.begin()));
                ids = intersection;
            }

            std::vector<std::vector<std::vector<Component*>>> all_comps(0);
            std::vector<int16_t> ids_vec(ids.begin(), ids.end());
            for(int i = 0; i < ids_vec.size(); i++){
                std::vector<std::vector<Component*>> comps(esys->typs.size());
                for(int j=0; j<esys->typs.size(); j++){
                    comps[j]=component_maps[esys->typs[j]]->get_all(ids_vec[i]);
                }
                all_comps.push_back(comps);
            }
            if(print_calls){
                std::cout << "Calling all-system " << esys->name << " with " << all_comps.size() << " different id's." << std::endl;
            }
            esys->update(ids_vec, this, all_comps);
        }
        
        else if (All_Component_System* wsys = dynamic_cast<All_Component_System*>(sys)){
            std::vector<std::vector<Component*>> comps(wsys->typs.size());
            for(int i = 0; i < wsys->typs.size(); i++){
                comps[i]=component_maps[wsys->typs[i]]->get_all_components();
            }
            if(print_calls){
                std::cout << "Calling world-system " << sys->name << " with: ";
                for(int j=0; j<esys->typs.size(); j++){
                    std::cout << comps[j][0]->name << "[" << comps[j].size() << "], ";
                }
                std::cout << std::endl;
            }
            wsys->update(this, comps);
        } 
    }

}

std::string ECS_manager::get_debug_table() {
    
    std::ostringstream ss;
    ss << "ID  |  Components\n";
    
    std::set<int16_t> all_ids = {};
    for (const auto &[key, value] : component_maps){
        auto ids = value->get_all_ids();
        all_ids.insert(ids.begin(), ids.end());
    }
    std::vector<int16_t> ids_vec(all_ids.begin(), all_ids.end());
    for(int i = 0; i < ids_vec.size(); i++){
        std::set<Component*> all_comps = {};
        for (const auto &[key, value] : component_maps){
            auto comps = value->get_all_components_with_id(ids_vec[i]);
            all_comps.insert(comps.begin(), comps.end());
        }
        ss << ids_vec[i] << ":     ";
        for(auto comp : all_comps){
            ss << "(" << comp->name << ", " << comp->component_id << "), ";
        }
        ss << std::endl;
    }
    std::cout << ss.str();
    return ss.str();
}

std::string ECS_manager::get_str(std::type_index typ){
    return typ_index_to_string[typ];
}