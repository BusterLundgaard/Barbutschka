#ifndef SYSTEM_H
#define SYSTEM_H

#include "defs.h"
#include <functional>
#include <set>
#include "component.h"
#include "events.h"
#include "flags.h"
#include "graphic_layers.h"

class Ecs_m;

struct System_data {
    Typ typ;
    System_data(Typ typ) : typ(typ) {}
    virtual ~System_data() = default;
};

struct System {
    std::string name;
    V<Typ> typs;
    GRAPHIC_LAYER graphic_layer;
    System(std::string name, V<Typ> typs, GRAPHIC_LAYER graphic_layer) : name(name), typs(typs), graphic_layer(graphic_layer) {}
    virtual ~System() = default;
};

struct Component_individual_system : public System {
    std::function<void( Component*)> update;
    Component_individual_system(std::string name, Typ typ, GRAPHIC_LAYER graphic_layer, std::function<void( Component* el)> update) : 
        System(name, V<Typ>{typ}, graphic_layer), 
        update(update) {}
};
struct Component_for_all_system : public System {
    std::function<void( V<Component*>)> update;
    Component_for_all_system(std::string name, Typ typ, GRAPHIC_LAYER graphic_layer, std::function<void( V<Component*> comps)> update) : 
        System(name, V<Typ>{typ}, graphic_layer), 
        update(update) {}
};
struct Entity_individual_system : public System {
    V<flag> flags = {};
    std::function<void( Id)> update = []( Id id){};
    std::function<void( Id)> first_frame = []( Id id){};
    Map<EVENT, std::function<void( Id, Event_data)>> event_callbacks;  
    Entity_individual_system(std::string name, V<Typ> typs, V<flag> flags, GRAPHIC_LAYER graphic_layer, std::function<void( Id)> update) : 
        System(name, typs, graphic_layer), flags(flags),
        update(update) {}
    Entity_individual_system(std::string name, V<Typ> typs, V<flag> flags, GRAPHIC_LAYER graphic_layer, std::function<void( Id)> first_frame, std::function<void( Id entity_id)> update) : 
        System(name, typs, graphic_layer), flags(flags),
        update(update), first_frame(first_frame) {}
    Entity_individual_system(std::string name, V<Typ> typs, V<flag> flags, GRAPHIC_LAYER graphic_layer, std::function<void( Id)> first_frame, Map<EVENT, std::function<void( Id, Event_data)>> event_callbacks, std::function<void( Id entity_id)> update): 
        System(name, typs, graphic_layer), flags(flags),
        update(update), first_frame(first_frame), event_callbacks(event_callbacks) {}
    Entity_individual_system(std::string name, V<Typ> typs, V<flag> flags, GRAPHIC_LAYER graphic_layer, Map<EVENT, std::function<void( Id, Event_data)>> event_callbacks, std::function<void( Id entity_id)> update): 
        System(name, typs, graphic_layer), flags(flags),
        update(update), event_callbacks(event_callbacks) {}
};
struct Entity_individual_system_with_data : public System {
    V<flag> flags = {};

    std::function<void( Id, System_data*)> update;
    std::function<void( Id)> first_frame;
    Map<EVENT, std::function<void( Id, Event_data)>> event_callbacks;  

    std::function<System_data*()> initialize_data;
    std::function<void(System_data*)> destroy_data;
    
    Entity_individual_system_with_data(
        std::function<System_data*()> initialize_data, 
        std::function<void(System_data*)> destroy_data, 
        std::string name, 
        V<Typ> typs, 
        V<flag> flags,
        GRAPHIC_LAYER graphic_layer,
        std::function<void( Id entity_id, System_data*)> update): 
        System(name, typs, graphic_layer), flags(flags),
        update(update), first_frame([]( Id id){}), event_callbacks({}), initialize_data(initialize_data), destroy_data(destroy_data) {}        
    Entity_individual_system_with_data(
        std::function<System_data*()> initialize_data, 
        std::function<void(System_data*)> destroy_data, 
        std::string name, 
        V<Typ> typs, 
        V<flag> flags,
        GRAPHIC_LAYER graphic_layer,
        std::function<void( Id entity_id)> first_frame, 
        std::function<void( Id entity_id, System_data*)> update): 
        System(name, typs, graphic_layer), 
        update(update), first_frame(first_frame), event_callbacks({}), initialize_data(initialize_data), destroy_data(destroy_data) {}
    Entity_individual_system_with_data(
        std::function<System_data*()> initialize_data, 
        std::function<void(System_data*)> destroy_data, 
        std::string name, 
        V<Typ> typs, 
        V<flag> flags,
        GRAPHIC_LAYER graphic_layer,
        Map<EVENT, std::function<void( Id, Event_data)>> event_callbacks,
        std::function<void( Id entity_id, System_data*)> update): 
        System(name, typs, graphic_layer), flags(flags),
        update(update), first_frame([]( Id id){}), event_callbacks(event_callbacks), initialize_data(initialize_data), destroy_data(destroy_data) {}  
    Entity_individual_system_with_data(
        std::function<System_data*()> initialize_data, 
        std::function<void(System_data*)> destroy_data, 
        std::string name, 
        V<Typ> typs, 
        V<flag> flags,
        GRAPHIC_LAYER graphic_layer,
        std::function<void( Id entity_id)> first_frame, 
        Map<EVENT, std::function<void( Id, Event_data)>> event_callbacks,
        std::function<void( Id, System_data*)> update): 
        System(name, typs, graphic_layer), flags(flags),
        update(update), first_frame(first_frame), event_callbacks(event_callbacks), initialize_data(initialize_data), destroy_data(destroy_data) {}
};

struct Entity_for_all_system : public System {
    std::function<void( std::set<Id>)> update;
    Entity_for_all_system(std::string name, V<Typ> typs, GRAPHIC_LAYER graphic_layer, std::function<void( std::set<Id> entity_ids)> update): 
        System(name, typs, graphic_layer), 
        update(update) {}
};

struct General_system : public System {
    std::function<void()> update;
    Map<EVENT, std::function<void( Event_data)>> event_callbacks;  
    General_system(std::string name, GRAPHIC_LAYER graphic_layer, std::function<void()> update, Map<EVENT, std::function<void( Event_data)>> event_callbacks) : 
        System(name, {}, graphic_layer),
        update(update), event_callbacks(event_callbacks) {};
};

struct General_system_with_data : public System {
    std::function<void( System_data*)> update;
    Map<EVENT, std::function<void( Event_data)>> event_callbacks;  
    General_system_with_data(std::string name, GRAPHIC_LAYER graphic_layer, std::function<void( System_data*)> update, Map<EVENT, std::function<void( Event_data)>> event_callbacks) : 
        System(name, {}, graphic_layer),
        update(update), event_callbacks(event_callbacks) {};
};

struct System_instance {
    System* sys;
    Id entity_id;
};
bool operator<(const System_instance& l, const System_instance& r); 

#endif