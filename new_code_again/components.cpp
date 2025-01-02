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
#include <deque>
#include <cstdint>
#include <cstring>
#include <set>

typedef int16_t Id; 
typedef std::type_index Typ;
template <typename T>
using V = std::vector<T>;
template <typename T1, typename T2>
using Map = std::unordered_map<T1, T2>;

// #define boilerplate(name_of_comp) const std::string name() const override {return #name_of_comp;}  const Typ typ() const override {return typeid(_ ## name_of_comp);}
// #define comp(name_of_comp) class _ ## name_of_comp : public Component { public: boilerplate(name_of_comp)  
// #define allow_multiples const bool multiples_allowed() override {return true;}  
// #define is_singleton const bool singleton() override {return true;}

// //Revel in the horror of overloaded macros!!! I copy pasted this and don't understand how it works
// #define CAT( A, B ) A ## B
// #define SELECT( NAME, NUM ) CAT( NAME ## _, NUM )

// #define GET_COUNT( _1, _2, _3, _4, _5, _6 /* ad nauseam */, COUNT, ... ) COUNT
// #define VA_SIZE( ... ) GET_COUNT( __VA_ARGS__, 6, 5, 4, 3, 2, 1 )

// #define VA_SELECT( NAME, ... ) SELECT( NAME, VA_SIZE(__VA_ARGS__) )(__VA_ARGS__)

// #define COMP( ... ) VA_SELECT( COMP, __VA_ARGS__)
// #define COMP_1(comp_name) comp(comp_name)
// #define COMP_3(comp_name, property_1_typ, property_1_name) comp(comp_name) property_1_typ property_1_name; _ ## comp_name (property_1_typ property_1_name) : property_1_name(property_1_name) {}
// #define COMP_5(comp_name, property_1_typ, property_1_name, property_2_typ, property_2_name) comp(comp_name) property_1_typ property_1_name; property_2_typ property_2_name; _ ## comp_name (property_1_typ property_1_name, property_2_typ property_2_name) : property_1_name(property_1_name), property_2_name(property_2_name) {} 

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
virtual ~Component() = default;

void set_entity_id(Id entity_id) {this->entity_id = entity_id;}
};


class _Sprite : public Component {
    public:
    Texture2D tex;

    _Sprite(Texture2D tex) : tex(tex) {};

    private:
    static int init; static int initialize() {
        metadata(typeid(_Sprite), "Sprite", sizeof(_Sprite));   
    return 0;}
};
int _Sprite::init = _Sprite::initialize();
static Typ __Sprite = typeid(_Sprite);


class _Transform : public Component {
    public:
    float x, y;
    _Transform(float x, float y) : x(x), y(y) {};

    private:
    static int init; static int initialize() {
        metadata(typeid(_Sprite), "Sprite", sizeof(_Sprite));   
    return 0;}
};
int _Transform::init = _Transform::initialize();
static Typ __Transform = typeid(_Transform);


class _Collider : public Component {
    public:
    float x, y, w, h;
    V<Id> hits;
    _Collider(float x, float y, float w, float h) : x(x), y(y), w(w), h(h), hits({}) {}

    private:
    static int init; static int initialize() {
        metadata(typeid(_Collider), "Collider", sizeof(_Collider));   
        meta_set_heap_management(typeid(_Collider),
            [](void* src, void* dst){
                _Collider* el = static_cast<_Collider*>(src);
                static_cast<_Collider*>(dst)->hits = *(new std::vector<Id>(el->hits));
            },
            [](void* src){
                delete &(static_cast<_Collider*>(src)->hits);
            }  
        );
    return 0;}
};
int _Collider::init = _Collider::initialize();
static Typ __Collider = typeid(_Collider);


class Component_list {
    private:
    const Typ typ;    
    void* data;
    void* to_be_added_data;
    int to_be_added_data_queue_size = 0;

    int search_index=0;

    bool check_id_comp(int index, Id comp_id){
        return static_cast<Component*>(get(index))->comp_id == comp_id;
    }
    bool check_id_entity(int index, Id entity_id){
        return static_cast<Component*>(get(index))->entity_id == entity_id;
    }

    public:
    int size=0;

    Component_list(Typ typ) : typ(typ) {
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
        return -1;
    }
    V<int> indexes_of_comps(std::set<Id> component_ids){
        V<int> indexes;
        for(int i = 0; i<size; i++){
            if(component_ids.find(static_cast<Component*>(get(i))->comp_id) != component_ids.end()){
                indexes.push_back(i);
            }
        }
        return indexes;
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
        return -1;
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
        return index_of_entity(entity_id) != -1;
    }

    Id entity_of_comp(Id comp_id){
        return static_cast<Component*>(get(index_of_comp(comp_id)))->entity_id;
    }
    
    void add(void* el){
        void* dest = to_be_added_data + to_be_added_data_queue_size*comps_metadata.at(typ).bytesize;
        std::memcpy(
            dest, 
            el, 
            comps_metadata.at(typ).bytesize
            );
        comps_metadata.at(typ).clone(el, dest);
        to_be_added_data_queue_size++;
    }
    void add_all_in_add_queue() {
        for(int i = 0; i < to_be_added_data_queue_size; i++){
            void* dest = data + size*comps_metadata.at(typ).bytesize;
            void* src = to_be_added_data + i*comps_metadata.at(typ).bytesize;
            std::memcpy(
                dest,
                src, 
                comps_metadata.at(typ).bytesize
            );
            comps_metadata.at(typ).clone(src, dest);
            comps_metadata.at(typ).deallocator(src);
            size++;
        }
        to_be_added_data_queue_size=0;
    }
    void remove(int index){
        void* el_src = data + index*comps_metadata.at(typ).bytesize;
        void* last_el_src = data + (size-1)*comps_metadata.at(typ).bytesize;

        comps_metadata.at(typ).deallocator(el_src);

        std::memcpy(
            el_src,
            last_el_src,
            comps_metadata.at(typ).bytesize
        );

        size--;
    }

    void* get(int index){
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
    std::function<void(Ecs_m&, Id)> update;
    Entity_individual_system(std::string name, V<Typ> typs, std::function<void(Ecs_m& em, Id entity_id)> update) : 
        System(name, typs), 
        update(update) {}
};
struct Entity_for_all_system : public System {
    std::function<void(Ecs_m&, std::set<Id>)> update;
    Entity_for_all_system(std::string name, V<Typ> typs, std::function<void(Ecs_m& em, std::set<Id> entity_ids)> update): 
        System(name, typs), 
        update(update) {}
};


class Ecs_m {
    private:
    Map<Typ, Component_list> comps = {};
    Map<Id, Typ> comp_id_to_typ;
    V<System*> systems;

    Map<System*, std::set<Id>> intersections={};

    std::deque<Id> delete_queue;

    void add_remove_all_in_queues() {
        for(auto [typ, comp_list] : comps){
            comp_list.add_all_in_add_queue();
        }
        while(delete_queue.size() > 0){
            Id comp_id = delete_queue.front();
            
            auto comp_list = comps[comp_id_to_typ.at(comp_id)];
            remove_intersections(comps.at(comp_id_to_typ.at(comp_id)).entity_of_comp(comp_id));  
            comp_list.remove(comp_list.index_of_comp(comp_id));
            comp_id_to_typ.erase(comp_id);

            delete_queue.pop_back();
        }
    }

    void add_intersections(Id entity_id) {
        for(auto [sys, entities] : intersections){
            for(Typ typ : sys->typs){
                if(!comps.at(typ).entity_exists(entity_id)){
                    goto dont_add;
                }
            }
            entities.insert(entity_id);
            continue;

            dont_add:
            continue;
        }
    }
    void remove_intersections(Id entity_id){
        for(auto [sys, entities] : intersections){
            for(Typ typ : sys->typs){
                if(!comps.at(typ).entity_exists(entity_id)){
                    goto remove;
                }
            }
            continue;

            remove:
            entities.erase(entity_id);
            continue;
        }
    }

    public:
    Ecs_m() {
        for(int i = 0; i < systems.size(); i++){
            if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(systems[i])){
                intersections.insert({systems[i], {}});
            }
            if(Entity_for_all_system* sys = dynamic_cast<Entity_for_all_system*>(systems[i])){
                intersections.insert({systems[i], {}});
            }
        }
    }

    //remmember you need to do the add/remove queue here!
    void remove(Id comp_id){
        delete_queue.push_front(comp_id);
    }

    template <typename T>
    void add(T& comp, Id entity_id){
        if(comps.find(typeid(T)) == comps.end()){
            comps.insert({typeid(T), Component_list(typeid(T))});
        }
        comps.at(typeid(T)).add(static_cast<void*>(&comp));  
        static_cast<Component&>(comp).set_entity_id(entity_id);
        comp_id_to_typ.insert({static_cast<Component&>(comp)->comp_id, typeid(T)});
        add_intersections(entity_id);
    }

    template <typename T>
    T& get(Id component_id){
        return static_cast<T&>(*comps.at(typeid(T)).get_from_comp(component_id));
    }

    template <typename T>
    T& get(void* el){
        return static_cast<T&>(*el);
    }

    template <typename T>
    V<T*> get_all(V<void*> els){
        V<T*> comp_typ_pointers(comp_indexes.size());
        for(int i = 0; i < els.size(); i++){
            comp_typ_pointers[i] = static_cast<T*>(els[i]);
        }
        return comp_typ_pointers;
    }

    template <typename T>
    T& get_from_entity(Id entity_id){
        return static_cast<T&>(*comps.at(typeid(T)).get_from_entity(entity_id)); 
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
    T& get_sibling(Id comp_id){
        Id entity_id = comps.at(comp_id_to_typ.at(comp_id)).entity_of_comp(comp_id);
        return *static_cast<T*>(comps.at(typeid(T)).get_from_entity(entity_id))
    }

    template <typename T>
    T& get_singleton(){
        return *static_cast<T*>(comps.at(typeid(T)).get(0));
    }

    void update() {
        for(int i = 0; i < systems.size(); i++){
            if(Entity_individual_system* sys = dynamic_cast<Entity_individual_system*>(systems[i])){
                for(Id entity_id : intersections.at(systems[i])){
                    sys->update(*this, entity_id);
                }   
                continue;
            }
            if(Entity_for_all_system* sys = dynamic_cast<Entity_for_all_system*>(systems[i])){
                sys->update(*this, intersections.at(systems[i]));
                continue;
            }
            if(Component_individual_system* sys = dynamic_cast<Component_individual_system*>(systems[i])){
                auto lst = comps.at(sys->typs.at(0));
                for(int j = 0; j < lst.size; j++){
                    sys->update(*this, lst.get(j));
                }                
            }
            if(Component_for_all_system* sys = dynamic_cast<Component_for_all_system*>(systems[i])){
                auto lst = comps.at(sys->typs.at(0));
                V<void*> els(lst.size);
                for(int j = 0; j < lst.size; j++){
                    els[i] = lst.get(j);
                }  
                sys->update(*this, els);
            }
        }
        add_remove_all_in_queues();
    }
};
