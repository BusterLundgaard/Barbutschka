#ifndef SYSTEM_H
#define SYSTEM_H

#include "defs.h"
#include <functional>
#include <set>
#include "component.h"
#include "events.h"

class Ecs_m;

struct System_data {
    Typ typ;
    System_data(Typ typ) : typ(typ) {}
    virtual ~System_data() = default;
};

struct System {
    std::string name;
    V<Typ> typs;
    System(std::string name, V<Typ> typs) : name(name), typs(typs) {}
    virtual ~System() = default;
};

struct Component_individual_system : public System {
    std::function<void(Ecs_m&, Component*)> update;
    Component_individual_system(std::string name, Typ typ, std::function<void(Ecs_m& em, Component* el)> update) : 
        System(name, V<Typ>{typ}), 
        update(update) {}
};
struct Component_for_all_system : public System {
    std::function<void(Ecs_m&, V<Component*>)> update;
    Component_for_all_system(std::string name, Typ typ, std::function<void(Ecs_m& em, V<Component*> comps)> update) : 
        System(name, V<Typ>{typ}), 
        update(update) {}
};
struct Entity_individual_system : public System {
    std::function<void(Ecs_m&, Id)> update = [](Ecs_m& em, Id id){};
    std::function<void(Ecs_m&, Id)> first_frame = [](Ecs_m& em, Id id){};
    Map<Event, std::function<void(Ecs_m&, Id)>> event_callbacks;  
    Entity_individual_system(std::string name, V<Typ> typs, std::function<void(Ecs_m& em, Id entity_id)> update) : 
        System(name, typs), 
        update(update) {}
    Entity_individual_system(std::string name, V<Typ> typs, std::function<void(Ecs_m& em, Id entity_id)> first_frame, std::function<void(Ecs_m& em, Id entity_id)> update) : 
        System(name, typs), 
        update(update), first_frame(first_frame) {}
    Entity_individual_system(std::string name, V<Typ> typs, std::function<void(Ecs_m& em, Id entity_id)> first_frame, Map<Event, std::function<void(Ecs_m&, Id entity_id)>> event_callbacks, std::function<void(Ecs_m& em, Id entity_id)> update): 
        System(name, typs), 
        update(update), first_frame(first_frame), event_callbacks(event_callbacks) {}
    Entity_individual_system(std::string name, V<Typ> typs, Map<Event, std::function<void(Ecs_m&, Id entity_id)>> event_callbacks, std::function<void(Ecs_m& em, Id entity_id)> update): 
        System(name, typs), 
        update(update), event_callbacks(event_callbacks) {}
};
struct Entity_individual_system_with_data : public System {
    std::function<void(Ecs_m&, Id, System_data*)> update;
    std::function<void(Ecs_m&, Id)> first_frame;
    Map<Event, std::function<void(Ecs_m&, Id)>> event_callbacks;  

    std::function<System_data*()> initialize_data;
    std::function<void(System_data*)> destroy_data;
    
    Entity_individual_system_with_data(
        std::function<System_data*()> initialize_data, 
        std::function<void(System_data*)> destroy_data, 
        std::string name, 
        V<Typ> typs, 
        std::function<void(Ecs_m& em, Id entity_id, System_data*)> update): 
        System(name, typs), 
        update(update), first_frame([](Ecs_m& em, Id id){}), event_callbacks({}), initialize_data(initialize_data), destroy_data(destroy_data) {}        
    Entity_individual_system_with_data(
        std::function<System_data*()> initialize_data, 
        std::function<void(System_data*)> destroy_data, 
        std::string name, 
        V<Typ> typs, 
        std::function<void(Ecs_m& em, Id entity_id)> first_frame, 
        std::function<void(Ecs_m& em, Id entity_id, System_data*)> update): 
        System(name, typs), 
        update(update), first_frame(first_frame), event_callbacks({}), initialize_data(initialize_data), destroy_data(destroy_data) {}
    Entity_individual_system_with_data(
        std::function<System_data*()> initialize_data, 
        std::function<void(System_data*)> destroy_data, 
        std::string name, 
        V<Typ> typs, 
        Map<Event, std::function<void(Ecs_m&, Id)>> event_callbacks,
        std::function<void(Ecs_m& em, Id entity_id, System_data*)> update): 
        System(name, typs), 
        update(update), first_frame([](Ecs_m& em, Id id){}), event_callbacks(event_callbacks), initialize_data(initialize_data), destroy_data(destroy_data) {}  
    Entity_individual_system_with_data(
        std::function<System_data*()> initialize_data, 
        std::function<void(System_data*)> destroy_data, 
        std::string name, 
        V<Typ> typs, 
        std::function<void(Ecs_m& em, Id entity_id)> first_frame, 
        Map<Event, std::function<void(Ecs_m&, Id)>> event_callbacks,
        std::function<void(Ecs_m& em, Id entity_id, System_data*)> update): 
        System(name, typs), 
        update(update), first_frame(first_frame), event_callbacks(event_callbacks), initialize_data(initialize_data), destroy_data(destroy_data) {}
};

struct Entity_for_all_system : public System {
    std::function<void(Ecs_m&, std::set<Id>)> update;
    Entity_for_all_system(std::string name, V<Typ> typs, std::function<void(Ecs_m& em, std::set<Id> entity_ids)> update): 
        System(name, typs), 
        update(update) {}
};

struct System_instance {
    System* sys;
    Id entity_id;
};
bool operator<(const System_instance& l, const System_instance& r); 

#endif