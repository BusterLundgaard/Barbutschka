
#ifndef COMPONENTS_H
#define COMPONENTS_H

#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <typeindex>
#include <set>
#include "utils.h"
#include <raylib.h>

#define make_list(comp) static ComponentMap<comp> comp ## _COMPONENT_LIST 
#define add_to_map(comp) component_lists[typeid(comp)]= &comp ## _COMPONENT_LIST;
#define entity_individual_signature [](int id, ECS_manager* em, std::vector<std::vector<Component*>> comps)
#define param(typ, var) typ* var = static_cast<typ*>

//Definitions and implementations:
//-----------------------------------
#pragma region defs_and_implementations
static int fresh_id = 0;

class Component {
    public:
    int16_t component_id;
    std::string name;
    bool allows_multiplicity;
    Component(std::string name, bool allows_multiplicity) : name(name), component_id(generate_id()), allows_multiplicity(allows_multiplicity) {};
    Component(std::string name) : name(name), component_id(generate_id()), allows_multiplicity(false) {};
    virtual ~Component() = default;

    private:
    static int generate_id() {
        return fresh_id++;
    }
};

class BaseComponentMap {
    public:
    BaseComponentMap() = default;
    //Maps from component id's (unique) to entity id's (not unique)
    std::unordered_map<int16_t, int16_t> id_map;

    virtual ~BaseComponentMap() = default;
    virtual void add(Component* c, int16_t id)=0;
    virtual Component* get(int16_t comp_id)=0;
    virtual std::vector<Component*> get_all(int16_t entity_id)=0;
    virtual void remove(int16_t component_id)=0;
    virtual void remove_all(int16_t entity_id)=0;

    virtual bool contains_entity_id(int16_t entity_id)=0;
    virtual bool contains_comp_id(int16_t comp_id)=0;
    virtual std::set<int16_t> get_all_ids()=0;
    virtual std::vector<Component*> get_all_components()=0;
    virtual std::set<Component*> get_all_components_with_id(int16_t entity_id)=0;
};

class ECS_manager;

//System:
struct System {
    std::string name;
    std::vector<std::type_index> typs;
    System(std::string name, std::vector<std::type_index> typs) : name(name), typs(typs) {}
    virtual ~System() {}
};

struct Entity_System_Individual : public System {
    std::function<void(int, ECS_manager*, std::vector<std::vector<Component*>>)> update;
    Entity_System_Individual(std::string name, std::vector<std::type_index> typs, std::function<void(int, ECS_manager*, std::vector<std::vector<Component*>>)> update) : System(name, typs), update(update) {}
};
struct Entity_System_All : public System {
    std::function<void(std::vector<int16_t>, ECS_manager*, std::vector<std::vector<std::vector<Component*>>>)> update;
    Entity_System_All(std::string name, std::vector<std::type_index> typs, std::function<void(std::vector<int16_t>, ECS_manager*, std::vector<std::vector<std::vector<Component*>>>)> update) : System(name, typs), update(update) {}
};
struct All_Component_System : public System {
    std::function<void(ECS_manager*, std::vector<std::vector<Component*>>)> update;
    All_Component_System(std::string name, std::vector<std::type_index> typs, std::function<void(ECS_manager*, std::vector<std::vector<Component*>>)> update) : System(name, typs), update(update) {}
};

//ComponentMap<T> header and implementation
template <typename T>
class ComponentMap : public BaseComponentMap {
    std::vector<T> components;
    int iteration_index = 0;
    std::set<int16_t> added_ids = {};

    void remove_index(int index);

    public:
    ComponentMap();
    virtual void add(Component* c, int16_t id) override;

    // You could optimize the get functions by having a search index that is saved every call
    virtual Component* get(int16_t id) override;
    virtual std::vector<Component*> get_all(int16_t entity_id) override;
    virtual void remove(int16_t component_id) override;

    virtual void remove_all(int16_t entity_id) override;

    virtual bool contains_entity_id(int16_t entity_id);
    virtual bool contains_comp_id(int16_t comp_id);

    virtual std::set<int16_t> get_all_ids();
    virtual std::vector<Component*> get_all_components();
    virtual std::set<Component*> get_all_components_with_id(int16_t entity_id);
    
};

template <typename T>
void ComponentMap<T>::remove_index(int index){
    T temp = components[components.size()-1];
    components[components.size()-1] = components[index];
    components[index]=temp;
    components.pop_back();
}

template <typename T>
ComponentMap<T>::ComponentMap() : BaseComponentMap() {}

template <typename T>
void ComponentMap<T>::add(Component* c, int16_t id) {
    auto stuff = static_cast<T*>(c);
    components.push_back(*stuff);
    id_map[c->component_id] = id;
    added_ids.insert(id);
}

template <typename T>
Component* ComponentMap<T>::get(int16_t id) {
    for(int i = 0; i < components.size(); i++){
        auto as_comp = static_cast<Component*>(&components[i]);  
        if(id_map[as_comp->component_id] == id){
            return as_comp;
        }
    }
    return nullptr;
}

template <typename T>
std::vector<Component*> ComponentMap<T>::get_all(int16_t entity_id) {
    std::vector<Component*> comps = {};
    for(int i = 0; i < components.size(); i++){
        auto as_comp = static_cast<Component*>(&components[i]);  
        if(id_map[as_comp->component_id] == entity_id){
            comps.push_back(as_comp);
        }
    }
    return comps;
}

template <typename T>
void ComponentMap<T>::remove(int16_t component_id){
    for(int i = 0; i < components.size(); i++){
        auto as_comp = static_cast<Component*>(&components[i]);
        if(as_comp->component_id == component_id){
            id_map.erase(component_id);

            int entity_id = id_map[as_comp->component_id];
            int amount_with_id = get_all(entity_id).size();
            if(amount_with_id == 1){
                added_ids.erase(entity_id);
            }

            return;
        }
    }
}

template <typename T>
void ComponentMap<T>::remove_all(int16_t entity_id) {
    std::vector<int> to_remove = {};
    for(int i = 0; i < components.size(); i++){
        auto as_comp = static_cast<Component*>(&components[i]);  
        if(id_map[as_comp->component_id] == entity_id){
            id_map.erase(as_comp->component_id);
            to_remove.push_back(i);
        }
    }
    for(int index : to_remove){
        remove_index(index);
    }
}

template <typename T>
bool ComponentMap<T>::contains_entity_id(int16_t entity_id) {
    for(const auto& [key, value] : id_map){
        if(value == entity_id){return true;}
    }
    return false;
}

template <typename T>
bool ComponentMap<T>::contains_comp_id(int16_t comp_id) {
    return id_map.find(comp_id) != id_map.end()
}

template <typename T>
std::set<int16_t> ComponentMap<T>::get_all_ids() {
    return added_ids;
}

template <typename T>
std::vector<Component*> ComponentMap<T>::get_all_components() {
    std::vector<Component*> comps(components.size());
    for(int i = 0; i < components.size(); i++){
        comps[i] = static_cast<Component*>(&components[i]);
    }
    return comps;
}

template <typename T>
std::set<Component*> ComponentMap<T>::get_all_components_with_id(int16_t entity_id) {
    std::set<Component*> comps = {};
    for(int i = 0; i < components.size(); i++){
        auto as_comp = static_cast<Component*>(&components[i]);  
        if(id_map[as_comp->component_id] == entity_id){
            comps.insert(as_comp);
        }
    }
    return comps;
}

#pragma endregion defs_and_implementations
//-----------------------------------

//Components
class _Sprite : public Component {
    public:
    Texture2D tex;
    _Sprite(std::string path) : Component("Sprite"), tex(LoadTexture(path.c_str())) {}
};

class _Transform : public Component {
    public:
    int x, y;
    _Transform(int x, int y) : Component("Transform"), x(x), y(y) {}
};

class _Velocity : public Component {
    public:
    int x, y;
    _Velocity(int x, int y) : Component("Velocity"), x(x), y(y) {}
};

class _HitCollider : public Component {
    //entity id's of other hit colliders it hit
    public:
    int x, y, w, h;
    int gx, gy;

    std::set<int16_t> hit; 
    std::set<int16_t> p_hit; 
    
    _HitCollider(int offset_x, int offset_y, int w, int h) : 
        Component("HitCollider", true), 
        x(offset_x),
        y(offset_y),
        w(w),
        h(h),
        hit({}), p_hit({}) {}
    
    std::set<int16_t> entered() {
        std::set<int16_t> entered = {};
        for(auto h : hit){
            if(!exists(p_hit, h)){
                entered.insert(h);
            }
        }
        return entered;
    }
    std::set<int16_t>& inside() {
        return hit;
    }
    std::set<int16_t> left() {
        std::set<int16_t> left = {};
        for(auto h : p_hit){
            if(!exists(hit, h)){
                left.insert(h);
            }
        }
        return left;
    }
};

make_list(_Sprite);
make_list(_Transform);
make_list(_Velocity);
make_list(_HitCollider);

static void set_component_lists(std::unordered_map<std::type_index, BaseComponentMap*>& component_lists) {
    add_to_map(_Sprite);
    add_to_map(_Transform);
    add_to_map(_Velocity);
    add_to_map(_HitCollider);
};

#endif





// #define map(comp) static ComponentMap<comp> COMPONENTS_ ## comp = {}
// #define typ_map(comp) {typeid(comp), &COMPONENTS_ ## comp}

// class A : public Component {
//     public:
//     int x;
//     A(int a) : Component(), x(a) {}
// };

// class B : public Component {
//     public:
//     int y;
//     std::vector<int> items;
//     B(int b) : Component(), y(b), items({}) {}
// };

// map(A);
// map(B);

// static std::unordered_map<std::type_index, BaseComponentMap*> typ_map = {
//     typ_map(A),
//     typ_map(B)
// };

