#ifndef ECS_M_H
#define ECS_M_H

#include <map>
#include <deque>
#include "defs.h"
#include "system.h"
#include "utils.h"
#include "component_list.h"

class Ecs_m {
    private:
    Map<Id, Id> comp_id_to_entity_id;
    Map<Id, V<Id>> entity_id_to_comp_id;

    Map<Id, Typ> comp_id_to_typ;
    Map<Id, V<Typ>> entity_id_to_typs;

    V<System*> systems;
    std::map<System_instance, System_data*> systems_data = {};

    Map<System*, std::set<int16_t>> intersections={};
    
    std::set<System_instance> first_frame_systems ={};

    std::deque<Id> add_queue;
    std::deque<Id> delete_queue;

    void add_all_in_queues();
    void remove_all_in_queues();

    void add_intersections(Id entity_id, Typ type_added);
    void remove_intersections(Id entity_id);

    public:
    Map<Typ, Component_list> comps = {};

    Ecs_m(V<System*> systems);
    
    void update();

    //REMOVERS:
    void remove(Id comp_id);
    // Remove first component of Type typ from entity:
    void remove_typ(Id entity_id, Typ typ);
    
    //ADDERS:
    void add(Component* el, Id entity_id, Typ typ);
    template <typename T>
    void add(T comp, Id entity_id){
        add(static_cast<Component*>(&comp), entity_id, typeid(T));
    }

    //GETTERS:
    template <typename T>
    T* get_from_comp(Id component_id){
        return (static_cast<T*>(comps.at(typeid(T)).get_from_comp(component_id)));
    }
    Component* get_from_entity(Id entity_id, Typ typ){
        return static_cast<Component*>(comps.at(typ).get_from_entity(entity_id)); 
    }
    template <typename T>
    T* get_from_entity(Id entity_id){
        return static_cast<T*>(comps.at(typeid(T)).get_from_entity(entity_id)); 
    }
    template <typename T>
    V<T*> get_all_from_entity(Id entity_id){
        auto list = comps.at(typeid(T));
        V<int> comp_indexes = list.indexes_of_entity(entity_id);
        V<T*> comp_typ_pointers(comp_indexes.size());
        for(int i = 0; i < comp_indexes.size(); i++){
            comp_typ_pointers[i] = static_cast<T*>(list.get(comp_indexes[i]));
        }
        return comp_typ_pointers;
    }
    template <typename T>
    T* get_sibling(Id comp_id){
        Id entity_id = comps.at(comp_id_to_typ.at(comp_id)).entity_of_comp(comp_id);
        return static_cast<T*>(comps.at(typeid(T)).get_from_entity(entity_id));
    }
    template <typename T>
    T* get_singleton(){
        return static_cast<T*>(comps.at(typeid(T)).get(0));
    }
    Id get_entity_id(Id comp_id);

    bool has_type(Id entity_id, Typ typ);

    //DEBUG:
    void print_table();
};


#endif