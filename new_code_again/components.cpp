//systems: either they are component-wise or entity-wise:
//in component wise, the lambda just takes either a component id or a vector of component ids (for all versus for one)
//in entity-wise, the lambda just takes either an entity id or a vector of entity ids
//they are themselves responsible for calling a function on em that gets the actual pointers (the components themselves)

//because we can't have vectors of references, you have to do pointers throughout for consistency ;(
//eh, vastly most systems are for-one, so for those we can use references fine. 

#include <iostream>
#include <typeindex>
#include <functional>
#include <raylib.h>
#include <string>
#include <unordered_map>
#include <map>
#include <deque>
#include <cstdint>
#include <cstring>
#include <set>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>
#include <optional>
#include <iomanip>
#include "exceptions.h"

#define init_comp(compname) int compname::init = compname::initialize(); static Typ _ ## compname = typeid(compname);
#define init_meta static int init; static int initialize() 
#define default_meta(compname) metadata(typeid(compname), #compname, sizeof(compname)); 
#define manual_heap_meta(compname) \
    meta_set_heap_management( \
        typeid(compname), \
        [](void* src, void* dst) { \
            new (dst) compname(static_cast<compname*>(src)); \
        }, \
        [](void* src) { \
            compname* el = static_cast<compname*>(src); \
            el->~compname(); \
        } \
    )
#define manage_system_data(systemname) \ 
[](){\
    return static_cast<System_data*>(new systemname);\
},\
[](System_data* data_pointer){\
    delete static_cast<systemname*>(data_pointer);\
},

const int GAME_WIDTH = 256;
const int GAME_HEIGTH = 176;
const int BLOCKS_X = 16;
const int BLOCKS_Y = 11;
const int BLOCK_SIZE = 16;

typedef int16_t Id; 
typedef std::type_index Typ;
template <typename T>
using V = std::vector<T>;
template <typename T1, typename T2>
using Map = std::unordered_map<T1, T2>;


// Util 
template <typename T>
bool exists(std::set<T> s, T el){
    return s.find(el) == s.end();
}

template <typename T>
bool exists(V<T> v, T el){
    return std::find(v.begin(), v.end(), el) != v.end();
}

template <typename T>
int count(V<T> v, T el){
    return std::count(v.begin(), v.end(), el);
}

template<typename T>
std::vector<T> filter(const std::vector<T>& v, std::function<bool(T)> f){
    std::vector<T> filtered;
    for(int i = 0; i < v.size(); i++){
        if(f(v[i])){
            filtered.push_back(v[i]);
        }
    }
    return filtered;
}

Vector2 operator+(const Vector2& lhs, const Vector2& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

// Overload - operator
Vector2 operator-(const Vector2& lhs, const Vector2& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

// Optional: Overload << operator for easy printing
std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

Vector2& operator*=(Vector2& vec, float scalar) {
    vec.x *= scalar;
    vec.y *= scalar;
    return vec; // Return the modified vector
}

Vector2& operator/=(Vector2& vec, float scalar) {
    vec.x /= scalar;
    vec.y /= scalar;
    return vec; // Return the modified vector
}

float magnitude(const Vector2& vec){
    return sqrt(vec.x*vec.x + vec.y*vec.y);
}

void rotate_vec(Vector2& vec, float angle){
    Vector2 tmp = {vec.x, vec.y};
    vec.x = tmp.x * std::cos(angle) - tmp.y * std::sin(angle);
    vec.y = tmp.x * std::sin(angle) + tmp.y * std::cos(angle);
}

void draw_vector(int x, int y, Vector2 vec, Color color){
    DrawLine(x, y, x+vec.x, y+vec.y, color);
}

void draw_arrow(int x, int y, int to_x, int to_y, Color color){
    //draw base
    Vector2 vec = Vector2{static_cast<float>(to_x-x), static_cast<float>(to_y-y)};
    draw_vector(x, y, vec, color);
    
    //draw tail
    rotate_vec(vec, 0.52f);
    vec *= -6.0f/magnitude(vec);
    draw_vector(to_x, to_y, vec, color);
    rotate_vec(vec, -0.52f*2);
    draw_vector(to_x, to_y, vec, color);
}
//

struct Component_metadata {
    int bytesize;
    std::string name;
    
    bool has_heap_memory=false;
    std::function<void(void*, void*)> clone = [](void* src, void* dst){};
    std::function<void(void*)> deallocator = [](void* el){};
    
    bool is_singleton=false;
    bool allow_multiple=false;
    int max_elements = 10;
};
static Map<Typ, Component_metadata> comps_metadata = {};
static Map<Id, std::string> named_entities = {};

void metadata(Typ typ, std::string name, int bytesize) {
    comps_metadata.insert({typ, {bytesize, name}}); 
}
void meta_set_singleton(Typ typ, bool is_singleton) {
    comps_metadata.at(typ).is_singleton = is_singleton;
}
void meta_set_allow_multiple(Typ typ, bool allow_multiple){
    comps_metadata.at(typ).allow_multiple = allow_multiple;
}
void meta_set_max_elements(Typ typ, int max_elements){
    comps_metadata.at(typ).max_elements = max_elements;
}
void meta_set_heap_management(
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

class Component_list {
    private:
    const Typ typ;    
    void* data;
    void* to_be_added_data;
    int to_be_added_data_queue_size;

    int search_index=0;

    bool check_id_comp(int index, Id comp_id){
        Component* comp = static_cast<Component*>(get(index));
        return static_cast<Component*>(get(index))->comp_id == comp_id;
    }
    bool check_id_entity(int index, Id entity_id){
        Component* comp = static_cast<Component*>(get(index));
        return static_cast<Component*>(get(index))->entity_id == entity_id;
    }

    bool exists_in_queue(Id entity_id){
        for(int i = 0; i < to_be_added_data_queue_size; i++){
            Component* comp = static_cast<Component*>(to_be_added_data + i*comps_metadata.at(typ).bytesize);
            if(comp->entity_id == entity_id){return true;}
        }
        return false;
    }

    public:
    int size;

    Component_list(Typ typ) : typ(typ), size(0), to_be_added_data_queue_size(0) {
        data = malloc(comps_metadata.at(typ).bytesize*comps_metadata.at(typ).max_elements);

        //This could technically be optimized into something that scales in size and isn't just max_elements, but it'll do for now
        to_be_added_data = malloc(comps_metadata.at(typ).bytesize*comps_metadata.at(typ).max_elements);
    }

    int index_of_comp(Id component_id) {
        //Look forward from search_indexes (most often this is how we'll iterate)
        for(int i = search_index; i<size; i++){
            if(check_id_comp(i, component_id)){
                search_index=i;
                return i;
            }
        }
        //Look backwards if didn't find it in front:
        for(int i = search_index; i >= 0; i--){
            if(check_id_comp(i, component_id)){
                search_index=i;
                return i;
            }
        }
        throw NullPtrException("Could not find component of type " + comps_metadata.at(typ).name + " with component id " + std::to_string(component_id));
    }
    int index_of_entity(Id entity_id) {
        //Look forward from search_indexes (most often this is how we'll iterate)
        for(int i = search_index; i<size; i++){
            if(check_id_entity(i, entity_id)){
                search_index=i;
                return i;
            }
        }
        //Look backwards if didn't find it in front:
        for(int i = search_index; i >= 0; i--){
            if(check_id_entity(i, entity_id)){
                search_index=i;
                return i;
            }
        }
        throw NullPtrException("Could not find component of type " + comps_metadata.at(typ).name + " with entity id " + std::to_string(entity_id));
    }
    V<int> indexes_of_entity(Id entity_id){
        V<int> indexes;
        for(int i = 0; i<size; i++){
            if(check_id_entity(i, entity_id)){
                indexes.push_back(i);
            }
        }
        return indexes;
    }

    bool entity_exists(Id entity_id){
        if(exists_in_queue(entity_id)){return true;}
        for(int i = search_index; i<size; i++){
            if(check_id_entity(i, entity_id)){
                search_index=i;
                return true;
            }
        }
        for(int i = search_index; i >= 0; i--){
            if(check_id_entity(i, entity_id)){
                search_index=i;
                return true;
            }
        }
        return false;
    }

    Id entity_of_comp(Id comp_id){
        return static_cast<Component*>(get(index_of_comp(comp_id)))->entity_id;
    }
    
    void add(void* el){
        void* dest = to_be_added_data + to_be_added_data_queue_size*comps_metadata.at(typ).bytesize;
        if(comps_metadata.at(typ).has_heap_memory){
            comps_metadata.at(typ).clone(el, dest);
        } else {
            std::memcpy(
                dest, 
                el, 
                comps_metadata.at(typ).bytesize
                );
        }
        to_be_added_data_queue_size++;
    }
    void add_all_in_add_queue() {
        for(int i = 0; i < to_be_added_data_queue_size; i++){
            void* dest = data + size*comps_metadata.at(typ).bytesize;
            void* src = to_be_added_data + i*comps_metadata.at(typ).bytesize;
            if(comps_metadata.at(typ).has_heap_memory){
                comps_metadata.at(typ).clone(src, dest);
                comps_metadata.at(typ).deallocator(src);
            } else {
                std::memcpy(
                    dest,
                    src, 
                    comps_metadata.at(typ).bytesize
                );
            }
            size++;
        }
        to_be_added_data_queue_size=0;
    }
    void remove(int index){
        void* el_src = data + index*comps_metadata.at(typ).bytesize;
        void* last_el_src = data + (size-1)*comps_metadata.at(typ).bytesize;

        if(comps_metadata.at(typ).has_heap_memory){
            comps_metadata.at(typ).deallocator(el_src);
            comps_metadata.at(typ).clone(last_el_src, el_src);
        } else {
            std::memcpy(
                el_src,
                last_el_src,
                comps_metadata.at(typ).bytesize
            );
        }
        
        size--;
    }

    void* get(int index){
        int bitsize = comps_metadata.at(typ).bytesize;
        return data + index*comps_metadata.at(typ).bytesize;
    }
    void* get_from_comp(Id comp_id){
        return get(index_of_comp(comp_id));
    }
    void* get_from_entity(Id entity_id){
        return get(index_of_entity(entity_id));
    }
};

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

int frame = 0;

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

    void add_all_in_queues() {
        while(add_queue.size() > 0){
            Id comp_id = add_queue.front();

            comps.at(comp_id_to_typ.at(comp_id)).add_all_in_add_queue();
            add_intersections(comp_id_to_entity_id.at(comp_id), comp_id_to_typ.at(comp_id));

            add_queue.pop_front();
        }
    }

    void remove_all_in_queues(){
        while(delete_queue.size() > 0){
            Id comp_id = delete_queue.front();
            
            Typ typ = comp_id_to_typ.at(comp_id);
            Id entity_id = comps.at(typ).entity_of_comp(comp_id);

            comps.at(typ).remove(comps.at(typ).index_of_comp(comp_id));            
            comp_id_to_typ.erase(comp_id);
            comp_id_to_entity_id.erase(comp_id);
            auto comp_ids = &entity_id_to_comp_id.at(entity_id);
            comp_ids->erase(std::find(comp_ids->begin(), comp_ids->end(), comp_id));
            auto typs = &entity_id_to_typs.at(entity_id);
            typs->erase(std::find(typs->begin(), typs->end(), typ));

            remove_intersections(entity_id);  

            //Should this maybe be set to pop_front() as well? No idea why different from add queue ....
            delete_queue.pop_front();
        }
    }

    void add_intersections(Id entity_id, Typ type_added) {
        if(count(entity_id_to_typs.at(entity_id), type_added) > 1){return;} //Check if this type of element has already been added before

        for(auto it = intersections.begin(); it != intersections.end(); ++it){
            if(!exists(it->first->typs, type_added)){goto dont_add;} // Is the type we are adding even relevant for the system in question? 

            for(Typ typ : it->first->typs){
                if(comps.find(typ) == comps.end()){
                    goto dont_add;
                }
                if(!exists(entity_id_to_typs.at(entity_id), typ)){
                    goto dont_add;
                }
            }
            it->second.insert(entity_id);
            first_frame_systems.insert({it->first, entity_id});
            
            if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(it->first)){
                System_data* data = sys->initialize_data();
                if(data != nullptr){
                    systems_data.insert({{it->first, entity_id}, data});
                }
            }

            dont_add:
            continue;
        }
    }
    void remove_intersections(Id entity_id){
        for(auto it = intersections.begin(); it != intersections.end(); ++it){
            for(Typ typ : it->first->typs){
                if(!exists(entity_id_to_typs.at(entity_id), typ)){
                    goto remove;
                }
            }
            continue;

            remove:
            it->second.erase(entity_id);
            if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(it->first)){
                if(auto data_it = systems_data.find({it->first, entity_id}); data_it != systems_data.end()){
                    sys->destroy_data(data_it->second);
                }
            }

            continue;
        }
    }

    public:
    Map<Typ, Component_list> comps = {};

    Ecs_m(V<System*> systems) : systems(systems) {
        for(int i = 0; i < systems.size(); i++){
            if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(systems[i])){
                intersections.insert({systems[i], {}});
            }
            if(Entity_for_all_system* sys = dynamic_cast<Entity_for_all_system*>(systems[i])){
                intersections.insert({systems[i], {}});
            }
            if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(systems[i])){
                intersections.insert({systems[i], {}});
            }

            for(Typ typ : systems[i]->typs){
                comps.insert({typ, Component_list(typ)});
            }
        }
    }

    void remove(Id comp_id){
        if(comp_id_to_entity_id.find(comp_id) != comp_id_to_entity_id.end()){
            delete_queue.push_front(comp_id);
        }
    }

    // Remove first component of Type typ from entity:
    void remove_typ(Id entity_id, Typ typ){
        for(Id id : entity_id_to_comp_id.at(entity_id)){
            if(comp_id_to_typ.at(id) == typ){
                remove(id);
                return;
            }
        }
        std::cout << "\033[31mWARNING: TRIED TO REMOVE TYPE " 
                  << comps_metadata.at(typ).name 
                  << " FROM ENTITY " << std::to_string(entity_id) 
                  << " BUT THERE WAS NONE!\033[0m" << std::endl;
    }

    void add(Component* el, Id entity_id, Typ typ){
        if(comps.find(typ) == comps.end()){
            comps.insert({typ, Component_list(typ)});
        }
        el->set_entity_id(entity_id);
        comps.at(typ).add(static_cast<void*>(el)); 
        add_queue.push_front(el->comp_id);
        
        comp_id_to_typ.insert({el->comp_id, typ});
        comp_id_to_entity_id.insert({el->comp_id, entity_id});
        if(entity_id_to_comp_id.find(entity_id) == entity_id_to_comp_id.end()){
            entity_id_to_comp_id.insert({entity_id, {el->comp_id}});
            entity_id_to_typs.insert({entity_id, {typ}});
        } else {
            entity_id_to_comp_id.at(entity_id).push_back(el->comp_id);
            entity_id_to_typs.at(entity_id).push_back(typ);
        }
    }

    template <typename T>
    void add(T comp, Id entity_id){
        add(static_cast<Component*>(&comp), entity_id, typeid(T));
    }

    template <typename T>
    T* get_from_comp(Id component_id){
        return (static_cast<T*>(comps.at(typeid(T)).get_from_comp(component_id)));
    }

    template <typename T>
    V<T*> convert_all(V<void*> els){
        V<T*> comp_typ_pointers(els.size());
        for(int i = 0; i < els.size(); i++){
            comp_typ_pointers[i] = static_cast<T*>(els[i]);
        }
        return comp_typ_pointers;
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

    Id get_entity_id(Id comp_id){
        auto res = comp_id_to_entity_id.find(comp_id);
        if(res != comp_id_to_entity_id.end()){
            return res->second;
        }
        throw NullPtrException("Component with id " + std::to_string(comp_id) + " has no corresponding entity");
    }

    bool has_type(Id entity_id, Typ typ){
        return exists(entity_id_to_typs.at(entity_id), typ);
    }

    void update() {
        for(System_instance sys_inst : first_frame_systems){
            if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(sys_inst.sys)){
                sys->first_frame(*this, sys_inst.entity_id);
            }
            if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(sys_inst.sys)){
                sys->first_frame(*this, sys_inst.entity_id);
            }
        }
        first_frame_systems={};

        for(int i = 0; i < systems.size(); i++){
            if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(systems[i])){
                for(Id entity_id : intersections.at(systems[i])){
                    sys->update(*this, entity_id);
                }   
                continue;
            }
            if(Entity_individual_system_with_data* sys = dynamic_cast<Entity_individual_system_with_data*>(systems[i])){
                for(Id entity_id : intersections.at(systems[i])){
                    sys->update(*this, entity_id, systems_data.at({systems[i], entity_id}));
                }   
                continue;
            }
            if(Entity_for_all_system* sys = dynamic_cast<Entity_for_all_system*>(systems[i])){
                sys->update(*this, intersections.at(systems[i]));
                continue;
            }
            if(Component_individual_system* sys = dynamic_cast<Component_individual_system*>(systems[i])){
                for(int j = 0; j < comps.at(sys->typs.at(0)).size; j++){
                    sys->update(*this, comps.at(sys->typs.at(0)).get(j));
                }                
            }
            if(Component_for_all_system* sys = dynamic_cast<Component_for_all_system*>(systems[i])){
                Typ ty = sys->typs.at(0);
                auto lst = comps.at(sys->typs.at(0));
                V<void*> els(lst.size);
                for(int j = 0; j < lst.size; j++){
                    els[j] = lst.get(j);
                }  
                sys->update(*this, els);
            }
        }
        add_all_in_queues();
        remove_all_in_queues();
    }


    void print_table() {
        // Gather all unique component types
        std::set<Typ> all_types;
        for (const auto& [entity_id, types] : entity_id_to_typs) {
            all_types.insert(types.begin(), types.end());
        }

        // Print the header row
        std::cout << std::setw(10) << "Entity ID";
        for (const Typ& typ : all_types) {
            std::cout << std::setw(20) << comps_metadata.at(typ).name;
        }
        std::cout << "\n";

        // Print a separator
        std::cout << std::string(10, '-') << "+";
        for (size_t i = 0; i < all_types.size(); ++i) {
            std::cout << std::string(20, '-') << "+";
        }
        std::cout << "\n";

        // Print each entity's row
        for (const auto& [entity_id, comp_ids] : entity_id_to_comp_id) {
            std::cout << std::setw(10);
            auto entity_name = named_entities.find(entity_id);
            if(entity_name != named_entities.end()){
                std::cout << entity_name->second << " | " << entity_id;
            } else {
                std::cout << entity_id;
            }

            for (const Typ& typ : all_types) {
                // Find all component IDs of this type for the current entity
                std::vector<Id> matching_ids;
                for (const Id& comp_id : comp_ids) {
                    if (comp_id_to_typ.at(comp_id) == typ) {
                        matching_ids.push_back(comp_id);
                    }
                }

                // Format the cell with the list of component IDs
                std::string cell_content = "[";
                for (size_t i = 0; i < matching_ids.size(); ++i) {
                    cell_content += std::to_string(matching_ids[i]);
                    if (i < matching_ids.size() - 1) {
                        cell_content += ", ";
                    }
                }
                cell_content += "]";

                std::cout << std::setw(20) << cell_content;
            }
            std::cout << "\n";
        }
    }

};

class _Sprite : public Component {
    public:
    Texture2D tex;

    _Sprite(std::string path) : tex(LoadTexture(path.c_str())) {};

    private:
    init_meta {
        default_meta(_Sprite)
        return 0;
    }
};
init_comp(_Sprite)


class _Transform : public Component {
    public:
    float x, y;
    float px, py;
    _Transform(float x, float y) : x(x), y(y) {};

    private:
    init_meta {
        default_meta(_Transform)
        return 0;
    }
};
init_comp(_Transform)

class _Velocity : public Component {
    public:
    float x, y;
    _Velocity(float x, float y) : x(x), y(y) {};

    private:
    init_meta {
        default_meta(_Velocity)
        return 0;
    }
};
init_comp(_Velocity)


class _Collider : public Component {
    public:
    float x, y, w, h;
    float gx, gy;

    bool hits_terrain, adjustable, solid;

    std::set<Id> hit; 
    std::set<Id> p_hit; 
    bool hit_terrain, p_hit_terrain;

    _Collider(float x, float y, float w, float h, bool hits_terrain, bool adjustable, bool solid) : 
        x(x), y(y), w(w), h(h), 
        hits_terrain(hits_terrain), adjustable(adjustable), solid(solid),
        gx(0), gy(0), hit({}), p_hit({}), hit_terrain(false), p_hit_terrain(0)
        {}
    _Collider(_Collider* col) : 
        Component(col->comp_id, col->entity_id), 
        x(col->x), y(col->y), w(col->w), h(col->h), 
        hits_terrain(col->hits_terrain), adjustable(col->adjustable), solid(col->solid), 
        gx(-5), gy(-5), hit({}), p_hit({}), hit_terrain(false), p_hit_terrain(0)
        {}

    std::set<Id> entered() {
        std::set<Id> entered = {};
        for(auto h : hit){
            if(!exists(p_hit, h)){
                entered.insert(h);
            }
        }
        return entered;
    }
    std::set<Id>& inside() {
        return hit;
    }
    std::set<Id> left() {
        std::set<Id> left = {};
        for(auto h : p_hit){
            if(!exists(hit, h)){
                left.insert(h);
            }
        }
        return left;
    }

    bool is_inside(float ox, float oy, float ow, float oh) {
        return ox+ow > gx && 
               ox < gx + w && 
               oy + oh > gy &&
               oy < gy + h; 
    }

    bool hits_solid(Ecs_m& em){
        for(Id id : hit){
            _Collider* col = em.get_from_comp<_Collider>(id);
            if(col->solid){return true;}
        }
        return false;
    }
    bool phits_solid(Ecs_m& em){
        for(Id id : p_hit){
            _Collider* col = em.get_from_comp<_Collider>(id);
            if(col->solid){return true;}
        }
        return false;
    }

    std::optional<Component*> get_typ_in_hits(Typ typ, std::set<Id>& hits, Ecs_m& em){
        for(Id id : hits){
            Id entity_id = em.get_entity_id(id);
            if(em.has_type(entity_id, typ)){
                return em.get_from_entity(entity_id, typ);
            }
        }
        return std::nullopt;
    }

    // Looks through all entities in hits and searches for a component in one of them of type typ. Returns first it finds. If no matches, returns nullptr.
    std::optional<Component*> get_typ_in_hits(Typ typ, Ecs_m& em){
        return get_typ_in_hits(typ, hit, em);
    }

    // Looks through all entities in p_hits and searches for a component in one of them of type typ. Returns first it finds. If no matches, returns nullptr.
    std::optional<Component*> get_typ_in_p_hits(Typ typ, Ecs_m& em){
        return get_typ_in_hits(typ, p_hit, em);
    }

    private:

    init_meta {
        default_meta(_Collider);
        manual_heap_meta(_Collider);
        return 0;
    }
};
init_comp(_Collider)


class _Level : public Component {
    public:
    Texture2D tilemap;
    u_int16_t grid[BLOCKS_Y][BLOCKS_X];

    _Level(std::string level_path, std::string tilemap_path) : 
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

    private:
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

    init_meta {
        default_meta(_Level);
        return 0;
    }
};
init_comp(_Level)


class _Moveable : public Component {
   public:
   int x;
   _Moveable(int x) : x(x) {};
   
   private:
   init_meta {
      default_meta(_Moveable);
      return 0;
   }
};
init_comp(_Moveable)


class _Player : public Component {
    public:
    Id ground_trigger_col_id;

    _Player(Id ground_trigger_col_id) : ground_trigger_col_id(ground_trigger_col_id) {}

    private:
    init_meta {
        default_meta(_Player);
        return 0;
    }
};
init_comp(_Player)


class _Oscillator : public Component {
    public:
    bool dir;
    int distance;
    float period;

    float time;

    bool forwards; //bool that tells you if it is in the forward or backwards cycle

    _Oscillator(bool dir, int distance, float period) : dir(dir), distance(distance), period(period), time(0), forwards(true) {};
    _Oscillator(_Oscillator* osc) : dir(osc->dir), distance(osc->distance), period(osc->period), time(osc->time), forwards(osc->forwards) {};

    void tick() {
        time += GetFrameTime();
    }
    void reset() {
        time = 0;
    }
    //THIS IS TEMP, WE HAVEN'T ACTUALLY COMPUTED SPEED YET!!
    float get_speed() {
        return (-1 + forwards*2)*(1);
    }

    private:
    init_meta {
        default_meta(_Oscillator);
        return 0;
    }
};
init_comp(_Oscillator)

struct animation {
    Texture2D spritesheet;
    float frame_speed;
    bool loops;
};

class _Animation_player : public Component {
    public:
    Map<std::string, animation> anims;
    std::string current_anim;
    float time_per_frame;
    int sprite_width, sprite_height;

    int offset_x, offset_y;

    float time;
    int frame;

    int flipped;

    _Animation_player(Map<std::string, animation> anims, float time_per_frame, int sprite_width, int sprite_height, int offset_x, int offset_y, std::string starting_animation) : 
        anims(anims), time_per_frame(time_per_frame), sprite_width(sprite_width), sprite_height(sprite_height), offset_x(offset_x), offset_y(offset_y),
        current_anim(starting_animation), time(0), frame(0), flipped(1) {}
    _Animation_player(_Animation_player* anim) : Component(anim->comp_id, anim->entity_id), anims(anim->anims), current_anim(anim->current_anim), time_per_frame(anim->time_per_frame), time(anim->time), frame(anim->frame), sprite_width(anim->sprite_width), sprite_height(anim->sprite_height), offset_x(anim->offset_x), offset_y(anim->offset_y), flipped(anim->flipped) {}

    void change_animation(std::string new_animation){
        if(!(current_anim == new_animation)){
            time=0;
            frame=0;
        }
        current_anim = new_animation;
    }

    void flip(){
        flipped = -flipped;
    }

    private:
    init_meta {
        default_meta(_Animation_player);
        meta_set_heap_management(typeid(_Animation_player), 
            [](void* src, void* dst) { 
                new (dst) _Animation_player(static_cast<_Animation_player*>(src)); 
                }, 
            [](void* src) { 
                _Animation_player* el = static_cast<_Animation_player*>(src); 
                el->~_Animation_player(); 
                });
        return 0;
    }
};
init_comp(_Animation_player)


Entity_individual_system sys_draw_sprite{
    "draw_sprite",
    {__Sprite, __Transform},
    [](Ecs_m& em, Id id){
        auto spr = em.get_from_entity<_Sprite>(id);
        auto t = em.get_from_entity<_Transform>(id);
        DrawTexture(spr->tex, t->x, GAME_HEIGTH - t->y - spr->tex.height, WHITE);
    }
};

Entity_individual_system sys_velocity{
    "velocity",
    {__Velocity, __Transform},
    [](Ecs_m& em, Id id){
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);  
        t->x += v->x;
        t->y += v->y;
    }
};

Entity_individual_system sys_update_collider_global_pos{
    "update_collider_global_pos", 
    {__Collider, __Transform}, 
    [](Ecs_m& em, Id id){
        auto t = em.get_from_entity<_Transform>(id);
        auto cols = em.get_all_from_entity<_Collider>(id);
        for(_Collider* col : cols){
            col->gx = t->x + col->x;
            col->gy = t->y + col->y;
        }
    }
};

Component_individual_system sys_set_prev_pos{
    "set_prev_pos",
    __Transform, 
    [](Ecs_m& em, void* el){
        auto t = static_cast<_Transform*>(el);
        t->px = t->x;
        t->py = t->y;
    }
};

Component_individual_system sys_draw_world{
    "draw_world",
    __Level, 
    [](Ecs_m& em, void* el){
        auto lvl = static_cast<_Level*>(el);

        int tilenum = 0;
        for(int i = 0; i < BLOCKS_Y; i++){
            for(int j = 0; j < BLOCKS_X; j++){
                tilenum = lvl->grid[i][j];
                if(tilenum == 0){continue;}
                DrawTextureRec(
                    lvl->tilemap, 
                    Rectangle{float(BLOCK_SIZE)*(tilenum-1), 0, BLOCK_SIZE, BLOCK_SIZE},
                    Vector2{float(BLOCK_SIZE)*j, float(BLOCK_SIZE)*i},
                    WHITE
                );
            }
        }
    }
};

Entity_individual_system sys_draw_animation{
    "draw_animation", 
    {__Animation_player, __Transform},
    [](Ecs_m& em, Id id){
        auto t = em.get_from_entity<_Transform>(id);
        auto anim = em.get_from_entity<_Animation_player>(id);

        anim->time += GetFrameTime();
        if(anim->time > anim->anims.at(anim->current_anim).frame_speed){
            anim->time=0;
            int amount_of_frames = anim->anims.at(anim->current_anim).spritesheet.width/anim->sprite_width;
            if(anim->anims.at(anim->current_anim).loops){
                anim->frame = (anim->frame + 1) % amount_of_frames;
            } else {
                anim->frame = std::min(amount_of_frames-1, anim->frame + 1);
            }
           
        }

        DrawTextureRec(
            anim->anims.at(anim->current_anim).spritesheet, 
            Rectangle{
                float(anim->sprite_width*anim->frame), 0, 
                float(anim->sprite_width), float(anim->sprite_height)},
            Vector2{t->x + anim->offset_x, GAME_HEIGTH - t->y - anim->offset_y - anim->sprite_height}, 
            WHITE
        );
    }
};


void update_collider_global_pos(Ecs_m& em, _Transform* t){
    auto cols = em.get_all_from_entity<_Collider>(t->entity_id);
    for(auto col : cols){
        col->gx = t->x + col->x;
        col->gy = t->y + col->y;
    }
}

Component_for_all_system sys_collision{
    "collision",
    __Collider, 
    [](Ecs_m& em, V<void*> els){
        auto cs = em.convert_all<_Collider>(els);
        _Level* lvl = em.get_singleton<_Level>();

        std::sort(cs.begin(), cs.end(), [](_Collider* a, _Collider* b) {  
            return a->gx < b->gx;
        });

        //Determine hits
        for(int i = 0; i < cs.size(); i++){
            cs[i]->p_hit = std::set<Id>(cs[i]->hit);
            cs[i]->hit = {};
            cs[i]->p_hit_terrain = cs[i]->hit_terrain;
            cs[i]->hit_terrain=false;
        }

        for(int i = 0; i < cs.size(); i++){
            //Other colliders
            std::vector<int> potential = {};
            int j = i-1;
            while(j >= 0 && cs[j]->gx + cs[j]->w > cs[i]->gx){
                potential.push_back(j);
                j--;                
            }
            j = i+1;
            while(j < cs.size() && cs[j]->gx < cs[i]->gx + cs[i]->w){
                potential.push_back(j);
                j++;                
            }
            for(int z : potential){
                if(cs[i]->is_inside(cs[z]->gx, cs[z]->gy, cs[z]->w, cs[z]->h)){
                    cs[z]->hit.insert(cs[i]->comp_id);
                    cs[i]->hit.insert(cs[z]->comp_id);
                }
            }

            //Static terrain
            if(!cs[i]->hits_terrain){continue;}

            if(lvl->is_inside(cs[i]->gx, cs[i]->gy, cs[i]->w, cs[i]->h)){
                cs[i]->hit_terrain=true;
            }
        }

        auto needs_adjustment = filter<_Collider*>(cs, [](_Collider* h){
            return h->adjustable && h->hit.size() > 0;
        });

        for(_Collider* col : needs_adjustment){
            _Transform* t = em.get_sibling<_Transform>(col->comp_id);
            float px = t->px + col->x;
            float py = t->py + col->y;


            //Adjust based on static environment collisions:
            // if(exists(col.hit, lvl.component_id)){
            //     if(lvl.is_inside(col.gx, py, col.w, col.h)){
            //         if(t.y-t.py > 0){ //Was above before
            //             t.y=(int(t.y)/BLOCK_SIZE)*BLOCK_SIZE;
            //         } else { // Was below before
            //             t.y=(int(t.y)/BLOCK_SIZE + 1)*BLOCK_SIZE;
            //         }
            //         col.gy = t.y + col.y;
            //     }
            //     if(lvl.is_inside(col.gy, px, col.w, col.h)){
            //         if(t.x - t.px > 0) { //Was going to right
            //             t.x=(int(t.x)/BLOCK_SIZE)*BLOCK_SIZE;
            //         }  else {
            //             t.x=(int(t.x)/BLOCK_SIZE+1)*BLOCK_SIZE;
            //         }
            //         col.gx = t.x + col.x;
            //     }
            // }
            
            //Adjust based on collisions with other solid colliders:
            std::vector<_Collider*> solids;
            for(auto hit : col->hit){
                _Collider* comp = em.get_from_comp<_Collider>(hit);
                if(comp->solid){solids.push_back(comp);}
            }
            for(auto solid : solids){
                _Transform* st = em.get_sibling<_Transform>(solid->comp_id);
                float delta_x = st->x - st->px;
                float delta_y = st->y - st->py;
                if(!solid->is_inside(px + delta_x, col->gy, col->w, col->h)){
                    if(t->x - t->px - delta_x > 0) { //to the left before
                        t->x -= (col->gx + col->w) - solid->gx; 
                    } else { //to the right before
                        t->x -= col->gx - (solid->gx + solid->w);
                    }
                }
                if(!solid->is_inside(col->gx, py + delta_y, col->w, col->h)){
                    if(t->y - t->py - delta_y > 0) { //below before
                        t->y -= (col->gy + col->h) - solid->gy; 
                    } else { // above before
                        t->y -= col->gy - (solid->gy + solid->h);
                    }   
                }

                update_collider_global_pos(em, t);
            }
        }
    }
};


Component_individual_system sys_DEBUG_draw_hit_collider(
    "DEBUG_draw_hit_collider",
    __Collider, 
    [](Ecs_m& em, void* el){
        auto col = static_cast<_Collider*>(el);

        DrawLine(col->gx,          GAME_HEIGTH - col->gy,            col->gx + col->w,  GAME_HEIGTH - (col->gy),          GREEN);
        DrawLine(col->gx,          GAME_HEIGTH - col->gy,            col->gx,           GAME_HEIGTH - (col->gy + col->h), GREEN);
        DrawLine(col->gx,          GAME_HEIGTH - (col->gy + col->h), col->gx + col->w,  GAME_HEIGTH - (col->gy + col->h), GREEN);
        DrawLine(col->gx + col->w, GAME_HEIGTH - col->gy,            col->gx + col->w,  GAME_HEIGTH - (col->gy + col->h), GREEN);

        // Draw hits:
        for (int16_t hit_id : col->hit) {
            auto hit_col = em.get_from_comp<_Collider>(hit_id);
            draw_arrow(col->gx, GAME_HEIGTH - col->gy, hit_col->gx, GAME_HEIGTH - hit_col->gy, RED);
        }
        if (col->hit_terrain) {
            DrawRectangle(0, 0, 10, 10, BLUE);
        }
    }
);


Entity_individual_system sys_player_movement{
    "player_movement",
    {__Player, __Transform, __Velocity, __Animation_player}, 
    [](Ecs_m& em, Id id){
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);
        auto p = em.get_from_entity<_Player>(id);
        auto anim = em.get_from_entity<_Animation_player>(id);
        
        _Collider* col = em.get_from_comp<_Collider>(p->ground_trigger_col_id);
        bool grounded = col->hits_solid(em);
        bool p_grounded = col->phits_solid(em);


        if(IsKeyDown(KEY_LEFT)){
            if(grounded){anim->change_animation("walk");}
            v->x = -1.0f;
        }
        else if(IsKeyDown(KEY_RIGHT)){ 
            if(grounded){anim->change_animation("walk");}
            v->x = 1.0f;
        }
        else{
            v->x = 0.0f;
            if(grounded){anim->change_animation("idle");}
        }

        auto moving_platform = col->get_typ_in_hits(__Oscillator, em);
        if(moving_platform){
            auto osc = static_cast<_Oscillator*>(*moving_platform);
            if(!osc->dir){ //Moves in x direction
                v->x += osc->get_speed();
            }
        }

        // if(grounded && !p_grounded){
        //     auto moving_platform = col->get_typ_in_hits(__Oscillator, em);
        //     if(moving_platform){
        //         std::cout << "Adding oscilattor!\n";
        //         auto osc_clone = _Oscillator(static_cast<_Oscillator*>(*moving_platform));
        //         em.add(osc_clone, id);
        //     }
        // }
        // else if(p_grounded && !grounded){
        //     auto p_moving_platform = col->get_typ_in_p_hits(__Oscillator, em);
        //     if(p_moving_platform){
        //         std::cout << "Removing oscilattor!\n";
        //         em.remove_typ(id, __Oscillator);
        //     }
        // }

        if(grounded && IsKeyDown(KEY_SPACE)){
            v->y = 2.0f;
            anim->change_animation("jump_start");
        }
        else if(p_grounded && !grounded){
            v->y = std::max(0.0f, v->y);
        }
        else if(!grounded){
            v->y -= 0.04f;
        }

    }
};


Entity_individual_system sys_oscillator_movement{
    "oscillator_movement",
    {__Oscillator, __Velocity},
    //First frame
    [](Ecs_m& em, Id id){ 
        auto v = em.get_from_entity<_Velocity>(id);
        auto ol = em.get_from_entity<_Oscillator>(id);
        if(ol->dir){
            v->y += 1;
        } else {
            v->x += 1;
        }
        
    },
    //Update
    [](Ecs_m& em, Id id){
        auto v = em.get_from_entity<_Velocity>(id);
        auto ol = em.get_from_entity<_Oscillator>(id);

        ol->tick();
        if(ol->time > ol->period){
            if(ol->dir){
                if(ol->forwards){v->y -= 2;} else {v->y += 2;}
            } else {
                if(ol->forwards){v->x -= 2;} else {v->x += 2;}
            }
            ol->forwards = !ol->forwards;
            ol->reset();
        }
    }
};


int main(){
    Ecs_m em({
        &sys_player_movement,
        
        &sys_oscillator_movement,

        &sys_velocity, 
        &sys_update_collider_global_pos, 
        &sys_collision,
        
        &sys_draw_sprite, 
        &sys_draw_animation,
        &sys_draw_world,
        &sys_DEBUG_draw_hit_collider, 
        &sys_set_prev_pos
        });

    // Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(GAME_WIDTH*4, GAME_HEIGTH*4, "Bartbushka");
	SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGTH);

    int entity_id = 1;

    // // rect2
    // em.add(_Transform(15.96747, 100), entity_id);
    // em.add(_Collider(0, 0, 106.02855, 30.7754431, false, false, true), entity_id);
    // entity_id++;

    // // rect3
    // em.add(_Transform(114.76625, 129), entity_id);
    // em.add(_Collider(0, 0, 7.2297816, 29.575159, false, false, true), entity_id);
    // entity_id++;

    // // rect4
    // em.add(_Transform(150.23247, 112), entity_id);
    // em.add(_Collider(0, 0, 26.989536, 45.281631, false, false, true), entity_id);
    // entity_id++;

    // // // rect5
    // em.add(_Transform(182.65874, 97), entity_id);
    // em.add(_Collider(0, 0, 26.989536, 45.281631, false, false, true), entity_id);
    // entity_id++;

    // // // rect6
    // em.add(_Transform(214.0717, 129), entity_id);
    // em.add(_Collider(0, 0, 33.576122, 33.628445, false, false, true), entity_id);
    // entity_id++;

    // // // rect7
    // em.add(_Transform(15, 128), entity_id);
    // em.add(_Collider(0, 0, 7, 29, false, false, true), entity_id);
    // entity_id++;

    // // // rect8
    // em.add(_Transform(195, 34), entity_id);
    // em.add(_Collider(0, 0, 37, 29, false, false, true), entity_id);
    // entity_id++;

    em.add(_Transform(10, 0), entity_id);
    em.add(_Collider(0, 0, 68, 19, false, false, true), entity_id);
    entity_id++;

    // em.add(_Level("../Tiled/test.csv", "../Tiled/basic.png"), entity_id);
    // entity_id++;

    // Collider 2
    em.add(_Transform(150, 0), entity_id);
    em.add(_Collider(0, 0, 123, 19, false, false, true), entity_id);
    entity_id++;

    // player
    em.add(_Transform(61.060257, 30), entity_id);
    em.add(_Velocity(0,0), entity_id);
    em.add(_Collider(0, 0, 15, 27, true, true, false), entity_id);
    _Collider ground_trigger(1, 0, 10, 3, false, false, false);
    em.add(ground_trigger, entity_id);
    em.add(_Player(ground_trigger.comp_id), entity_id);
        em.add(
        _Animation_player(
            {{"idle", {LoadTexture("../assets/tmp/Idle.png"), 1.0/5.0, true}}, 
             {"walk", {LoadTexture("../assets/tmp/Walk.png"), 1.0/15.0, true}},
             {"jump_start", {LoadTexture("../assets/tmp/JumpStart.png"), 1.0/4.0, false}},
             {"jump_fall", {LoadTexture("../assets/tmp/JumpFall.png"), 1.0/5.0, true}}
            }, 1.0/5.0, 20, 32, -4, 0, "idle"), 
        entity_id);
    named_entities.insert({entity_id, "Player"});
    entity_id++;

    // moving platform
    em.add(_Transform(90, 40), entity_id);
    em.add(_Collider(0, 0, 60, 15, false, false, true), entity_id);
    em.add(_Velocity(0,0), entity_id);
    em.add(_Oscillator(false, 100, 0.5f), entity_id);

    em.update();
    em.print_table();

    while(!WindowShouldClose()){
        float scale = std::min(
            (float)GetScreenWidth()/GAME_WIDTH, 
            (float)GetScreenHeight()/GAME_HEIGTH
        );

        BeginTextureMode(target);
        ClearBackground(WHITE);
        em.update();
        frame++;

        EndTextureMode();
        
        BeginDrawing();
        ClearBackground(WHITE);
        DrawTexturePro(
            target.texture, 
            Rectangle{
                0.0f, 0.0f, (float)GAME_WIDTH, -(float)GAME_HEIGTH
            },
            Rectangle{
                (GetScreenWidth()-float(GAME_WIDTH)*scale)*0.5f,  
                0,
                (float)GAME_WIDTH*scale,
                (float)GAME_HEIGTH*scale
            },
            Vector2{0,0}, 
            0.0f,
            WHITE
            );
        EndDrawing();
    }
    return 0;
}