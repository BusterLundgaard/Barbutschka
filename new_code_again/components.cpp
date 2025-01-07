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
#include <fstream>
#include <sstream>
#include <algorithm>
#include <math.h>

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
// int _Level::init = _Level::initialize();
// static Typ __Level = typeid(_Level);

// static int init; static int initialize() {
//         metadata(typeid(_Velocity), "Velocity", sizeof(_Velocity));
//         return 0;
//     }

// [](void* src, void* dst){
//     new (dst) _Collider(static_cast<_Collider*>(src));
// },
// [](void* src){
//     _Collider* el = static_cast<_Collider*>(src);
//     el->~_Collider();
// }  

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
        hits_terrain(hits_terrain), adjustable(adjustable), solid(solid) {}
    _Collider(_Collider* col) : 
        Component(col->comp_id, col->entity_id), 
        x(col->x), y(col->y), w(col->w), h(col->h), 
        hits_terrain(col->hits_terrain), adjustable(col->adjustable), solid(col->solid) {}

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
        return ox+ow > x && 
               ox < x + w && 
               oy + oh > y &&
               oy < y + h; 
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
        return index_of_entity(entity_id) != -1 || exists_in_queue(entity_id);
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
    Map<Id, Typ> comp_id_to_typ;
    V<System*> systems;

    Map<System*, std::set<int16_t>> intersections={};

    std::deque<Id> delete_queue;

    void add_all_in_queues() {
        for(auto it = comps.begin(); it != comps.end(); ++it){
            it->second.add_all_in_add_queue();
        }
    }

    void remove_all_in_queues(){
        while(delete_queue.size() > 0){
            Id comp_id = delete_queue.front();
            
            Typ typ = comp_id_to_typ.at(comp_id);
            Id entity_id = comps.at(typ).entity_of_comp(comp_id);
            comps.at(typ).remove(comps.at(typ).index_of_comp(comp_id));
            comp_id_to_typ.erase(comp_id);
            remove_intersections(entity_id);  

            delete_queue.pop_back();
        }
    }

    void add_intersections(Id entity_id, Typ new_typ_added) {
        for(auto it = intersections.begin(); it != intersections.end(); ++it){
            for(Typ typ : it->first->typs){
                if(comps.find(typ) == comps.end()){
                    goto dont_add;
                }
                if(typ != new_typ_added && !comps.at(typ).entity_exists(entity_id)){
                    goto dont_add;
                }
            }
            it->second.insert(entity_id);
            continue;

            dont_add:
            continue;
        }
    }
    void remove_intersections(Id entity_id){
        for(auto it = intersections.begin(); it != intersections.end(); ++it){
            for(Typ typ : it->first->typs){
                if(!comps.at(typ).entity_exists(entity_id)){
                    goto remove;
                }
            }
            continue;

            remove:
            it->second.erase(entity_id);
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
        }
    }

    void remove(Id comp_id){
        delete_queue.push_front(comp_id);
        comp_id_to_typ.insert(std::pair<int16_t, Typ>{0, typeid(_Transform)});
    }

    template <typename T>
    void add(T comp, Id entity_id){
        if(comps.find(typeid(T)) == comps.end()){
            comps.insert({typeid(T), Component_list(typeid(T))});
        }
        comp.set_entity_id(entity_id);
        comps.at(typeid(T)).add(static_cast<void*>(&comp));  
        comp_id_to_typ.insert(std::pair<int16_t, Typ>{comp.comp_id, typeid(T)});
        add_intersections(entity_id, typeid(T));
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

    void update() {
        add_all_in_queues();
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
                for(int j = 0; j < comps.at(sys->typs.at(0)).size; j++){
                    sys->update(*this, comps.at(sys->typs.at(0)).get(j));
                }                
            }
            if(Component_for_all_system* sys = dynamic_cast<Component_for_all_system*>(systems[i])){
                auto lst = comps.at(sys->typs.at(0));
                V<void*> els(lst.size);
                for(int j = 0; j < lst.size; j++){
                    els[j] = lst.get(j);
                }  
                sys->update(*this, els);
            }
        }
        remove_all_in_queues();
    }
};

Entity_individual_system sys_draw_sprite{
    "draw_sprite",
    {__Sprite, __Transform},
    [](Ecs_m& em, Id id){
        auto spr = em.get_from_entity<_Sprite>(id);
        auto t = em.get_from_entity<_Transform>(id);
        DrawTexture(spr->tex, t->x, t->y, WHITE);
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
                    Vector2{float(BLOCK_SIZE)*j,float(BLOCK_SIZE)*i},
                    WHITE
                );
            }
        }
    }
};

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
            cs[i]->p_hit = cs[i]->hit;
            cs[i]->hit = {};
            cs[i]->p_hit_terrain=cs[i]->hit_terrain;
            cs[i]->hit_terrain=false;

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
                if(cs[z]->gy < cs[i]->gy + cs[i]->h &&  
                   cs[z]->gy + cs[z]->h > cs[i]->gy) {
                    cs[z]->hit.insert(cs[i]->comp_id);
                    cs[i]->hit.insert(cs[z]->comp_id);
                   }
            }

            //Static terrain
            bool hits = cs[i]->hits_terrain;
            if(!hits){continue;}

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
                if(solid->is_inside(px - delta_x, col->gy, col->w, col->h)){
                    std::cout << "hit on x axis! \n";
                    if(delta_x < 0) { //to the left before
                        t->x -= col->gx + col->w - solid->gx;
                    } else { //to the right before
                        t->x += solid->gx + solid->w - col->gx;
                    }
                    col->gx = t->x + col->x;
                }
                if(solid->is_inside(col->gx, py - delta_y, col->w, col->h)){
                    std::cout << "hit on y axis! \n";
                    if(delta_y < 0) { //below before
                        t->y -= col->gy + col->h - solid->gy;  
                    } else { // above before
                        t->y += solid->gy + solid->h - col->gy;
                    }   
                    col->gy = t->y + col->y; 
                }
            }
        }


    }
};


Component_individual_system sys_DEBUG_draw_hit_collider(
    "DEBUG_draw_hit_collider",
    __Collider, 
    [](Ecs_m& em, void* el){
        auto col = static_cast<_Collider*>(el);

        DrawLine(col->gx,           col->gy,          col->gx + col->w,  col->gy,          GREEN);
        DrawLine(col->gx,           col->gy,          col->gx,           col->gy + col->h, GREEN);
        DrawLine(col->gx,           col->gy + col->h, col->gx + col->w,  col->gy + col->h, GREEN);
        DrawLine(col->gx + col->w,  col->gy,          col->gx + col->w,  col->gy + col->h, GREEN);

        // Draw hits:
        for (int16_t hit_id : col->hit) {
            auto hit_col = em.get_from_comp<_Collider>(hit_id);
            draw_arrow(col->gx, col->gy, hit_col->gx, hit_col->gy, RED);
        }
        if (col->hit_terrain) {
            DrawRectangle(0, 0, 10, 10, BLUE);
        }
    }
);


Entity_individual_system sys_debug_move{
    "debug_move",
    {__Moveable, __Velocity}, 
    [](Ecs_m& em, Id id){
        auto v = em.get_from_entity<_Velocity>(id);

        if (IsKeyDown(KEY_RIGHT)) {
            v->x = 1.0f; 
            v->y = 0;
        }
        if (IsKeyReleased(KEY_RIGHT)){v->x=0; v->y=0;}
        
        if (IsKeyDown(KEY_LEFT))  {
            v->x = -1.0f;
            v->y = 0;
        }
        if (IsKeyReleased(KEY_LEFT)){v->x=0; v->y=0;}
        
        if (IsKeyDown(KEY_UP))    {
            v->y = -1.0f;
            v->x = 0;
        }
        if (IsKeyReleased(KEY_UP)){v->x=0; v->y=0;}
        
        if (IsKeyDown(KEY_DOWN))  {
            v->y = 1.0f;
            v->x = 0;
        }
        if (IsKeyReleased(KEY_DOWN)){v->x=0; v->y=0;}
    }
};


int main(){
    Ecs_m em({
        &sys_velocity, 
        &sys_update_collider_global_pos, 
        &sys_debug_move,
        &sys_collision,
        &sys_draw_sprite, 
        &sys_draw_world,
        &sys_DEBUG_draw_hit_collider, 
        &sys_set_prev_pos
        });

    // Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(GAME_WIDTH*4, GAME_HEIGTH*4, "Hello Raylib");
	SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGTH);

    //Add your fun components here lol
    em.add(_Transform(40,70), 0);
    em.add(_Sprite("../assets/wabbit_alpha.png"), 0);

    em.add(_Transform(50,60), 1);
    em.add(_Sprite("../assets/wabbit_alpha.png"), 1);
    em.add(_Velocity(1,1), 1);

    em.add(_Level("../Tiled/test.csv", "../Tiled/basic.png"), 2);

    em.add(_Transform(70,50), 3);
    em.add(_Collider(0,0,44,33, false, false, false), 3);

    em.add(_Transform(56,42), 4);
    em.add(_Collider(0,0,34,54, false, false, false), 4);

    em.add(_Transform(136, 28), 5);
    em.add(_Collider(0,0,51, 50, false, false, false), 5);

    em.add(_Transform(169, 66), 6);
    em.add(_Collider(0,0,51, 50, false, false, false), 6);

    em.add(_Transform(13, 100), 7);
    em.add(_Collider(0,0,24, 21, true, false, false), 7);
    em.add(_Velocity(0,0), 7);
    em.add(_Moveable(10), 7);

    //em.add(_Collider(50, 60, 30, 30, false, false, false), 3);

    while(!WindowShouldClose()){
        float scale = std::min(
            (float)GetScreenWidth()/GAME_WIDTH, 
            (float)GetScreenHeight()/GAME_HEIGTH
        );

        BeginTextureMode(target);
        ClearBackground(BLACK);
        em.update();

        EndTextureMode();
        
        BeginDrawing();
        ClearBackground(BLACK);
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