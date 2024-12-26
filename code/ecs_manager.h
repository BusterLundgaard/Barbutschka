#ifndef ECS_MANAGER_H
#define ECS_MANAGER_H

#include <deque>
#include "components.h"

class ECS_manager {
    V<System*> systems;
    std::unordered_map<Typ, BaseComponentMap*> cms;

    std::unordered_map<Id, Id> comp_id_to_entity_id; 
    std::unordered_map<Id, Typ> comp_id_to_typ;

    std::deque<std::pair<Component*, Id>> add_queue;
    std::deque<Id> delete_queue;

    void add_in_add_queue();
    void delete_in_delete_queue();

    public:
    ECS_manager(
        std::unordered_map<Typ, BaseComponentMap*> component_lists,
        V<System*> systems
        );
    
    std::unordered_map<Id, V<Id>> entity_id_to_comp_ids;
    bool is_typ(Id comp_id, Typ typ);
    bool has_typ(Id entity_id, Typ typ);
    bool singleton_exists(Typ typ);

    template <typename T> 
    T& get(Id comp_id);

    template <typename T>
    V<T&> get_all(const V<Id>& comp_ids);

    template <typename T>
    T& get_singleton();

    template <typename T>
    T& get_sibling(Id comp_id);

    template <typename T>
    V<T&> get_all_siblings(Id comp_id);

    template <typename T>
    void add_comp(T comp, Id entity_id);

    void remove_comp(Id comp_id);

    void update();
};

template <typename T>
T& ECS_manager::get(Id comp_id){
    Component& the_comp = cms[typeid(T)]->get(comp_id);
    if(the_comp == nullptr){
        throw NullPtrException("Trying to get component with id " + std::to_string(comp_id) + " and type " + typ_index_to_string(typeid(T)) + ", but none have been added yet.");
    }
    T& the_real_comp = static_cast<T&>(the_comp);
    return static_cast<T&>(cms[typeid(T)]->get(comp_id));
}

template <typename T>
V<T&> ECS_manager::get_all(const V<Id>& comp_ids){
    V<T*> comps;
    for(auto id : comp_ids){
        comps.push_back(get<T>(id));
    }
    return comps;
}

template <typename T>
T& ECS_manager::get_singleton() {
    auto id_map = cms[typeid(T)]->comp_id_to_index;
    for(auto const [id, index] : id_map){
        return get<T>(id);
    }
    throw NullPtrException("Trying to get singleton of type " + typ_index_to_string(typeid(T)) + ", but none have been added yet");
}

template <typename T>
T& ECS_manager::get_sibling(Id comp_id){
    Id entity_id = comp_id_to_entity_id.at(comp_id);
    return static_cast<T&>(cms[typeid(T)]->get_with_entity_id(entity_id));
}

template <typename T>
V<T&> ECS_manager::get_all_siblings(Id comp_id){
    Id entity_id = comp_id_to_entity_id.at(comp_id);
    V<Component&> comps = cms[typeid(T)]->get_all_with_entity_id(entity_id);
    V<T&> comps_T(comps.size());
    for(int i = 0; i < comps.size(); i++){
        comps_T[i]=static_cast<T&>(comps[i]);
    }
    return comps_T;
}

template <typename T>
void ECS_manager::add_comp(T comp, Id entity_id){
    T* comp_pointer = new T(comp);
    add_queue.push_back({comp_pointer, entity_id});
    entity_id_to_comp_ids[entity_id]={};
}

#endif