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
void meta_set_deallocator(Typ typ, std::function<void(void*)> deallocator){
    comps_metadata.at(typ).deallocator=deallocator;
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


class Component_list {
    private:
    const Typ typ;    
    void* data;

    int search_index;

    public:
    int size;

    Component_list(Typ typ) : typ(typ) {
        data = malloc(comps_metadata.at(typ).bytesize*comps_metadata.at(typ).max_elements);
    }

    int index_of_comp(Id component_id) {
        // Search here
    }
    void index_of_entity(Id entity_id) {
        //Search here
    }

    void add(void* el){
        // do fancy stuff with the pointer here
    }
    void remove(int index){
        //
    }

    void* get(int index){

    }
    void* get(Id component_id) {

    }
    V<void*> get_all(Id entity_id){

    }
    V<void*> get_all(V<Id> comp_ids){

    }
};



class Ecs_m {
    private:
    Map<Typ, Component_list> comps = {};
    
    public:
    Ecs_m() {}

    //remmember you need to do the add/remove queue here!
    void remove(Typ typ, Id comp_id){
        comps.at(typ).remove(comps.at(typ).index_of_comp(comp_id));
    }

    template <typename T>
    void add(T& comp, Id entity_id){
        if(comps.find(typeid(T)) == comps.end()){
            comps.insert({typeid(T), Component_list(typeid(T))});
        }
        comps.at(typeid(T)).add(static_cast<void*>(&comp));  
        static_cast<Component&>(comp).set_entity_id(entity_id);
    }

    template <typename T>
    T& get(Id component_id){
        return static_cast<T&>(*comps.at(typeid(T)).get(component_id));
    }

    template <typename T>
    V<T*> get_all(V<Id> comp_ids){
        auto list = comps.at(typeid(T));
        V<void*> comp_pointers = list.get_all(comp_ids);
        V<T*> comp_typ_pointers(comp_pointers.size());
        for(int i = 0; i < comp_pointers.size(); i++){
            comp_typ_pointers[i] = static_cast<T*>(comp_pointers[i]);
        }
        return comp_typ_pointers;
    }



}



// class _Sprite : public Component{
//     public:
//     Texture2D tex;
//     _Sprite(Id entity_id, std::string path) : Component(entity_id), 
//         tex(LoadTexture(path.c_str())) {}
    
//     boilerplate(Sprite)


// };

// comp(Transform)
//     float x, y;
//     _Transform(float x, float y) : x(x), y(y) {}

// };

// COMP(Sprite, 
// Texture2D, tex
// ) 
// _Sprite(const std::string& path) : tex(LoadTexture(path.c_str())) {}
// };

// COMP(Transform, 
// float, x, 
// float, y
// ) 
// };

// class Component_List {
//     const Typ typ;
//     const int el_size;
//     void (*deallocator)(void*);
// };




