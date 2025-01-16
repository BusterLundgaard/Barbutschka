#ifndef SYSTEM_H
#define SYSTEM_H

#include "defs.h"
#include <functional>
#include <set>

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
    std::function<void(Ecs_m&, void*)> update;
    Component_individual_system(std::string name, Typ typ, std::function<void(Ecs_m& em, void* el)> update) : 
        System(name, V<Typ>{typ}), 
        update(update) {}
};
struct Component_for_all_system : public System {
    std::function<void(Ecs_m&, V<void*>)> update;
    Component_for_all_system(std::string name, Typ typ, std::function<void(Ecs_m& em, V<void*> comps)> update) : 
        System(name, V<Typ>{typ}), 
        update(update) {}
};
struct Entity_individual_system : public System {
    std::function<void(Ecs_m&, Id)> update = [](Ecs_m& em, Id id){};
    std::function<void(Ecs_m&, Id)> first_frame = [](Ecs_m& em, Id id){};
    Entity_individual_system(std::string name, V<Typ> typs, std::function<void(Ecs_m& em, Id entity_id)> update) : 
        System(name, typs), 
        update(update) {}
    Entity_individual_system(std::string name, V<Typ> typs, std::function<void(Ecs_m& em, Id entity_id)> first_frame, std::function<void(Ecs_m& em, Id entity_id)> update) : 
        System(name, typs), 
        update(update), first_frame(first_frame) {}
};
struct Entity_individual_system_with_data : public System {
    std::function<void(Ecs_m&, Id, System_data*)> update;
    std::function<void(Ecs_m&, Id)> first_frame;
    std::function<System_data*()> initialize_data;
    std::function<void(System_data*)> destroy_data;
    Entity_individual_system_with_data(
        std::function<System_data*()> initialize_data, 
        std::function<void(System_data*)> destroy_data, 
        std::string name, 
        V<Typ> typs, 
        std::function<void(Ecs_m& em, Id entity_id, System_data*)> update): 
        System(name, typs), 
        update(update), first_frame([](Ecs_m& em, Id id){}), initialize_data(initialize_data), destroy_data(destroy_data) {}
    Entity_individual_system_with_data(
        std::function<System_data*()> initialize_data, 
        std::function<void(System_data*)> destroy_data, 
        std::string name, 
        V<Typ> typs, 
        std::function<void(Ecs_m& em, Id entity_id)> first_frame, 
        std::function<void(Ecs_m& em, Id entity_id, System_data*)> update): 
        System(name, typs), 
        update(update), first_frame(first_frame), initialize_data(initialize_data), destroy_data(destroy_data) {}
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
bool operator<(const System_instance& l, const System_instance& r) {
    if (l.sys != r.sys) {
        return l.sys < r.sys; // Compare pointers for the `sys` member
    }
    return l.entity_id < r.entity_id; 
}

#endif