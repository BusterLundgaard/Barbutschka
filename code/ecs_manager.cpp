#include "ecs_manager.h"
#include "exceptions.h"

ECS_manager::ECS_manager(
        std::unordered_map<Typ, BaseComponentMap*> component_lists,
        V<System*> systems
        ) : cms(component_lists), systems(systems) {}

void ECS_manager::add_in_add_queue() {
    while(add_queue.size() > 0){
        auto pair = add_queue.front();
        Component* comp = pair.first;
        Id entity_id = pair.second;
        
        cms[comp->typ]->add(comp);
        comp_id_to_entity_id[comp->component_id]=entity_id;
        
        comp_id_to_typ.insert({comp->component_id, comp->typ});
        entity_id_to_comp_ids[entity_id].push_back(comp->component_id);

        //Big issue: it was created as a pointer to a T, but it is now a pointer to a Comp..
        //How do you cast it to the right type?
        delete pair.first;
        
        add_queue.pop_front();
    }
}

void ECS_manager::delete_in_delete_queue() {
    while(delete_queue.size() > 0){
        Id comp_id = delete_queue.front();
        
        cms[comp_id_to_typ.at(comp_id)]->remove(comp_id);
        comp_id_to_entity_id.erase(comp_id);
        comp_id_to_typ.erase(comp_id);

        Id entity_id = comp_id_to_entity_id[comp_id];
        entity_id_to_comp_ids[entity_id].pop_back();

        delete_queue.pop_back();
    }
}

void ECS_manager::remove_comp(Id comp_id){
    if(comp_id_to_entity_id.find(comp_id) == comp_id_to_entity_id.end()){
        throw RemoveNonExistingElement(comp_id);
    }
    delete_queue.push_back(comp_id);
}

bool ECS_manager::is_typ(Id comp_id, Typ typ){
    return comp_id_to_typ.at(comp_id) == typ;
}
bool ECS_manager::has_typ(Id entity_id, Typ typ){
    for(Id comp_id : entity_id_to_comp_ids[entity_id]){
        if(comp_id_to_typ.at(comp_id) == typ){
            return true;
        }
    }
    return false;
}
bool ECS_manager::singleton_exists(Typ typ) {
    return cms[typ]->comp_id_to_index.size() != 0;
}

void ECS_manager::update() {
    //Collect all component ids that you will need for all systems (good for cache)
    //For all systems, for all entities, for all typs, for all component ids on that entity with those typs
    V<V<V<V<Id>>>> comps(systems.size()); 
    for(auto const [entity_id, comp_ids] : entity_id_to_comp_ids) {
        for(int i = 0; i < systems.size(); i++){
            if(typeid(systems[i])==typeid(Component_system_individual*) || 
               typeid(systems[i])==typeid(Component_system_all*)){continue;}

            V<Id> comps_with_correct_typs;
            std::set<Typ> typs;
            for(Id comp_id : comp_ids){
                if(exists(systems[i]->param_typs, comp_id_to_typ.at(comp_id))){
                    typs.insert(comp_id_to_typ.at(comp_id));
                    comps_with_correct_typs.push_back(comp_id);
                }
            }
            if(typs.size() != systems[i]->param_typs.size()){continue;}

            std::unordered_map<Typ, int> typ_to_index;
            for(int j = 0; j < systems[i]->param_typs.size(); j++){
                typ_to_index[systems[i]->param_typs[j]]=j;
            }

            V<V<Id>> comps_for_entity(systems[i]->param_typs.size());
            
            for(Id comp_id : comps_with_correct_typs){
                comps_for_entity[typ_to_index[comp_id_to_typ.at(comp_id)]].push_back(comp_id);    
            }
            
            comps[i].push_back(comps_for_entity);
        }
    }

    //Actually call the systems using these ids 
    for(int i = 0; i < systems.size(); i++){
        if(Entity_system_individual* sys = dynamic_cast<Entity_system_individual*>(systems[i])){
            for(int j = 0; j < comps[i].size(); j++){
                sys->update(*this, comps[i][j]);
            }
        }
        else if(Entity_system_all* sys = dynamic_cast<Entity_system_all*>(systems[i])){
            if(comps[i].size() != 0){
                sys->update(*this, comps[i]);
            }
        }
        else if(Component_system_individual* sys = dynamic_cast<Component_system_individual*>(systems[i])){
            auto id_map = cms[sys->param_typs[0]]->comp_id_to_index;
            for(auto iter = id_map.begin(); iter != id_map.end(); ++iter)
            {
                sys->update(*this, iter->first);
            }
        }
        else if(Component_system_all* sys = dynamic_cast<Component_system_all*>(systems[i])){
            V<Id> all_comps;
            auto id_map = cms[sys->param_typs[0]]->comp_id_to_index;
            for(auto iter = id_map.begin(); iter != id_map.end(); ++iter)
            {
                all_comps.push_back(iter->first);
            }
            sys->update(*this, all_comps);
        }
    }

    add_in_add_queue();
    delete_in_delete_queue();
}
