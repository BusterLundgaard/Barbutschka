
#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "utils.h"
#include "constants.h"
#include "exceptions.h"

#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <typeindex>
#include <set>
#include <fstream>
#include <sstream>
#include <limits>
#include <raylib.h>

typedef int16_t Id;
typedef std::type_index Typ;
template <typename T>
using V = std::vector<T>;

#define make_list(comp) static ComponentMap<comp> comp ## _COMPONENT_LIST; static std::type_index _ ## comp = typeid(comp);  
#define add_to_map(comp) component_lists[typeid(comp)]= &comp ## _COMPONENT_LIST; typ_index_to_string[typeid(comp)] = #comp
#define entity_individual_signature [](ECS_manager& em, std::vector<std::vector<Id>> comps)
#define entity_all_signature [](ECS_manager& em, std::vector<std::vector<std::vector<Id>>> comps)
#define components_individual_signature [](ECS_manager& em, Id comp)
#define components_all_signature [](ECS_manager& em, std::vector<Id> comps)

static int fresh_id = 0;

class Component {
    public:
    Id component_id;
    Typ typ;
    bool allows_multiplicity;
    bool singleton;
    Component(Typ typ, bool allows_multiplicity, bool singleton) : typ(typ), component_id(generate_id()), allows_multiplicity(allows_multiplicity), singleton(singleton) {};
    Component(Typ typ, bool allows_multiplicity) : typ(typ), component_id(generate_id()), allows_multiplicity(allows_multiplicity), singleton(false) {};
    Component(Typ typ) : typ(typ), component_id(generate_id()), allows_multiplicity(false), singleton(false) {};
    virtual ~Component() = default;

    private:
    static int generate_id() {
        return fresh_id++;
    }
};

class BaseComponentMap {
    public:
    std::unordered_map<Id, int> comp_id_to_index;
    
    BaseComponentMap() = default;

    virtual ~BaseComponentMap() = default;

    virtual void add(Component* c)=0;
    virtual Component& get(Id comp_id)=0;
    virtual Component& get_with_entity_id(Id entity_id, std::unordered_map<Id, Id>& comp_id_to_entity_id)=0;
    virtual V<Component&> get_all_with_entity_id(Id entity_id, std::unordered_map<Id, Id>& comp_id_to_entity_id)=0;
    virtual void remove(Id comp_id)=0;
};

template <typename T>
class ComponentMap : public BaseComponentMap {
    V<T> components;

    public:
    ComponentMap();

    virtual void add(Component* c) override;
    virtual Component& get(Id comp_id) override;
    virtual Component& get_with_entity_id(Id entity_id, std::unordered_map<Id, Id>& comp_id_to_entity_id) override;
    virtual V<Component&> get_all_with_entity_id(Id entity_id, std::unordered_map<Id, Id>& comp_id_to_entity_id) override;
    virtual void remove(Id comp_id) override;
};

template<typename T>
ComponentMap<T>::ComponentMap() : BaseComponentMap() {}

template<typename T>
void ComponentMap<T>::add(Component* c) {
    auto comp = static_cast<T*>(c);
    components.push_back(*comp);
    comp_id_to_index[c->component_id]=components.size()-1;
}
template<typename T>
Component& ComponentMap<T>::get(Id comp_id) {
    return static_cast<Component&>(components[comp_id_to_index[comp_id]]);  
}
template<typename T>
Component& ComponentMap<T>::get_with_entity_id(Id entity_id, std::unordered_map<Id, Id>& comp_id_to_entity_id) {
    for(auto const [comp_id, index] : comp_id_to_index){
        if(comp_id_to_entity_id.at(comp_id) == entity_id){
            return static_cast<Component&>(components[comp_id_to_index[comp_id]]);
        }
    }
}  
template<typename T>
V<Component&> ComponentMap<T>::get_all_with_entity_id(Id entity_id, std::unordered_map<Id, Id>& comp_id_to_entity_id) {
    V<Component&> comps;
    for(auto const [comp_id, index] : comp_id_to_index){
        if(comp_id_to_entity_id.at(comp_id) == entity_id){
            comps.push_back(static_cast<Component&>(components[comp_id_to_index[comp_id]]));
        }
    }
    return comps;
}


template<typename T>
void ComponentMap<T>::remove(Id comp_id) {
    int index = comp_id_to_index[comp_id];    
    T temp = components[components.size() - 1];
    components[components.size()-1]=components[index];
    components[index]=temp;
    components.pop_back();

    comp_id_to_index.erase(comp_id);
    Id last_id = static_cast<Component*>(&temp)->component_id;
    comp_id_to_index[last_id]=index;
}

class ECS_manager;

struct System {
    std::string name;
    V<Typ> param_typs;
    System(std::string name, V<Typ> param_typs) : name(name), param_typs(param_typs) {}
    virtual ~System() {}
};

struct Entity_system_individual : public System {
    std::function<void(ECS_manager&, V<V<Id>>&)> update;
    Entity_system_individual(std::string name, V<Typ> param_typs, std::function<void(ECS_manager&, V<V<Id>>& comp_ids)> update) : System(name, param_typs), update(update) {}
};

struct Entity_system_all : public System {
    std::function<void(ECS_manager&, V<V<V<Id>>>&)> update;
    Entity_system_all(std::string name, V<Typ> param_typs, std::function<void(ECS_manager&, V<V<V<Id>>>&)> update) : System(name, param_typs), update(update) {}
};

struct Component_system_individual : public System {
    std::function<void(ECS_manager&, Id)> update;
    Component_system_individual(std::string name, Typ typ, std::function<void(ECS_manager&, Id)> update) : System(name, {typ}), update(update) {}
};

struct Component_system_all : public System {
    std::function<void(ECS_manager&, V<Id>&)> update;
    Component_system_all(std::string name, Typ param_typs, std::function<void(ECS_manager&, V<Id>&)> update) : System(name, {param_typs}), update(update) {}
};


//COMPONENTS:
class _Sprite : public Component {
    public:
    Texture2D tex;
    _Sprite(std::string path) : 
        Component(typeid(_Sprite)), 
        tex(LoadTexture(path.c_str())) {}
};

class _Transform : public Component {
    public:
    float x, y;
    float px, py;
    _Transform(float x, float y) : 
        Component(typeid(_Transform)), 
        x(x), y(y), 
        px(-std::numeric_limits<float>::infinity()), py(-std::numeric_limits<float>::infinity()) {}
};

class _Velocity : public Component {
    public:
    float x, y;
    _Velocity(float x, float y) : Component(typeid(_Velocity)), x(x), y(y) {}
};

class _HitCollider : public Component {
    //entity id's of other hit colliders it hit
    public:
    float x, y, w, h;
    float gx, gy;

    //Does this collider hit static terrain?
    bool hits_terrain;
    
    //Will the transform attached to this collider by adjusted by the collision system?
    bool adjustable;

    //Should this transform be considered in the adjustment collision system?
    bool solid;

    std::set<int16_t> hit; 
    std::set<int16_t> p_hit; 
    bool hit_terrain, p_hit_terrain;
    
    _HitCollider(float offset_x, float offset_y, float w, float h, bool hits_terrain, bool adjustable, bool solid) : 
        Component(typeid(_HitCollider), true), 
        x(offset_x),
        y(offset_y),
        w(w),
        h(h),
        hits_terrain(hits_terrain), adjustable(adjustable), solid(solid),
        hit({}), p_hit({}), hit_terrain(false), p_hit_terrain(false) {}

    _HitCollider(float offset_x, float offset_y, float w, float h, bool hits_terrain) :  _HitCollider(offset_x, offset_y, w, h, hits_terrain, false, false) {}
    _HitCollider(float offset_x, float offset_y, float w, float h) : _HitCollider(offset_x, offset_y, w, h, false, false, false) {}
    
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

    bool is_inside(float ox, float oy, float ow, float oh) {
        return ox+ow > x && 
               ox < x + w && 
               oy + oh > y &&
               oy < y + h; 
    }
};

//A collider that will hit SolidColliders and static terrain be adjusted by the adjustment system
class _Level : public Component {

    void set_grid(std::string level_path){
        std::ifstream level_file(level_path.c_str());
        std::string line;
        std::string number;
        int i = 0;
        while(std::getline(level_file, line)){
            std::istringstream line_input;
            line_input.str(line);
            int j = 0;
            while(std::getline(line_input, number, ',')){
                grid[i][j] = atoi(number.c_str())+1;       
                j++;
            }
            i++;
        }
    }

    public:
    Texture2D tilemap;
    u_int16_t grid[BLOCKS_Y][BLOCKS_X];
    
    _Level(std::string level_path, std::string tilemap_path) : 
        Component(typeid(_Level), false, true), 
        tilemap(LoadTexture(tilemap_path.c_str()))  
        {set_grid(level_path);} 

    bool is_inside(float x, float y, float w, float h){
        int bx0 = int(x)/BLOCK_SIZE;
        int bx1 = int(x+w)/BLOCK_SIZE+1;
        int by0 = int(y)/BLOCK_SIZE;
        int by1 = int(y+h)/BLOCK_SIZE+1;
        for(int i = bx0; i < bx1; i++){
            for(int j = by0; j < by1; j++){
                if(i >= 0 && j >= 0 && grid[j][i]){
                    return true;
                }
            }
        }
        return false;
    }
};

class _Moveable : public Component {
    public:
    _Moveable() : Component(typeid(_Moveable)) {}
};

static std::unordered_map<Typ, std::string> typ_index_to_string = {};

make_list(_Sprite);
make_list(_Transform);
make_list(_Velocity);
make_list(_HitCollider);
make_list(_Level);
make_list(_Moveable);

static void set_component_lists(std::unordered_map<std::type_index, BaseComponentMap*>& component_lists) {
    add_to_map(_Sprite);
    add_to_map(_Transform);
    add_to_map(_Velocity);
    add_to_map(_HitCollider);
    add_to_map(_Level);
    add_to_map(_Moveable);
};

#endif