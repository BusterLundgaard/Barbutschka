#ifndef COMPONENT_H
#define COMPONENT_H

#include <functional>
#include "defs.h"

struct Component_metadata {
    int bytesize;
    std::string name;
    std::string shorthand="";
    
    bool has_heap_memory=false;
    std::function<void(void*, void*)> clone = [](void* src, void* dst){};
    std::function<void(void*)> deallocator = [](void* el){};
    
    bool is_singleton=false;
    bool allow_multiple=false;
    int max_elements = 10;
};
extern Map<Typ, Component_metadata> comps_metadata;
extern Map<Id, std::string> named_entities;

inline void metadata(Typ typ, std::string name, int bytesize, std::string shorthand) {
    comps_metadata.insert({typ, {bytesize, name, shorthand}}); 
}
inline void meta_set_shorthand(Typ typ, std::string shorthand){
    comps_metadata.at(typ).shorthand = shorthand;
}
inline void meta_set_singleton(Typ typ, bool is_singleton) {
    comps_metadata.at(typ).is_singleton = is_singleton;
}
inline void meta_set_allow_multiple(Typ typ, bool allow_multiple){
    comps_metadata.at(typ).allow_multiple = allow_multiple;
}
inline void meta_set_max_elements(Typ typ, int max_elements){
    comps_metadata.at(typ).max_elements = max_elements;
}
inline void meta_set_heap_management(
    Typ typ, 
    std::function<void(void*, void*)> clone_fun, 
    std::function<void(void*)> deallocator_fun){
    
    //Something to consider here: Maybe the cloning step is slightly unnecesary? Can't we just make it a rule you can never add an element with a non-empty vector?
    //Well, you still have to actually create a new datastructure somewhere on the heap so...
    comps_metadata.at(typ).has_heap_memory=true;
    comps_metadata.at(typ).clone=clone_fun;
    comps_metadata.at(typ).deallocator=deallocator_fun;
}

static int fresh_id = 0;
class Component {
    public:
    Id comp_id, entity_id;

    Component() : comp_id(fresh_id++), entity_id(-1) {}
    Component(Id comp_id, Id entity_id) : comp_id(comp_id), entity_id(entity_id) {}
    virtual ~Component() = default;

    void set_entity_id(Id entity_id) {this->entity_id = entity_id;}
};

#endif