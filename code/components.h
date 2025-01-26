#pragma strict

#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "component.h"
#include "defs.h"
#include <raylib.h>
#include <fstream>
#include <sstream>
#include <set>
#include "utils.h"
#include "ecs_m.h"
#include <optional>
#include <algorithm>
#include <limits> 

class _Sprite : public Component {
    public:
    Texture2D tex;

    _Sprite(std::string path) : tex(LoadTexture(path.c_str())) {};

    static int initialize() {
        default_meta(_Sprite, "Sprt");
        return 0;
    }
};
static Typ __Sprite = typeid(_Sprite);


class _Transform : public Component {
    public:
    float x, y;
    float px, py;
    _Transform(float x, float y) : x(x), y(y) {};

    static int initialize() {
        default_meta(_Transform, "T");
        return 0;
    }
};
static Typ __Transform = typeid(_Transform);

class _Velocity : public Component {
    public:
    float x, y;
    _Velocity(float x, float y) : x(x), y(y) {};

    static int initialize() {
        default_meta(_Velocity, "V")
        return 0;
    }
};
static Typ __Velocity = typeid(_Velocity);


class _Collider : public Component {
    public:
    float x, y, w, h;
    float gx, gy;

    bool hits_terrain, adjustable, solid;
    bool is_slope, slope_dir;

    std::set<Id> hit; 
    std::set<Id> p_hit; 
    bool hit_terrain, p_hit_terrain;

    _Collider(float x, float y, float w, float h, bool hits_terrain, bool adjustable, bool solid) : 
        x(x), y(y), w(w), h(h), 
        hits_terrain(hits_terrain), adjustable(adjustable), solid(solid), is_slope(false), slope_dir(false),
        gx(0), gy(0), hit({}), p_hit({}), hit_terrain(false), p_hit_terrain(0)
        {}
    _Collider(float x, float y, float w, float h, bool hits_terrain, bool adjustable, bool solid, bool is_slope, bool slope_dir) : 
        x(x), y(y), w(w), h(h), 
        hits_terrain(hits_terrain), adjustable(adjustable), solid(solid), is_slope(is_slope), slope_dir(slope_dir),
        gx(0), gy(0), hit({}), p_hit({}), hit_terrain(false), p_hit_terrain(0)
        {}
    _Collider(_Collider* col) : 
        Component(col->comp_id, col->entity_id), 
        x(col->x), y(col->y), w(col->w), h(col->h), 
        hits_terrain(col->hits_terrain), adjustable(col->adjustable), solid(col->solid), is_slope(col->is_slope), slope_dir(col->slope_dir),
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
        if(!is_slope){
        return 
        ox+ow > gx && 
        ox < gx + w && 
        oy + oh > gy &&
        oy < gy + h; 
        }
        if(slope_dir){
        return
        ox + ow > gx - 1 &&
        ox + ow < gx + w + 1 && 
        oy > gy - 0.5 &&
        oy < gy + (ox+ow - gx) + 0.5;
        }
        return 
        ox > gx - 1 &&
        ox < gx + w + 1 && 
        oy > gy - 0.5 &&
        oy < gy + (gx+w-ox);
    }

    bool is_point_inside(float x, float y){
        return 
        x > gx && 
        x < gx + w &&
        y > gy &&
        y < gy + h;
    }

    V<Vector2> corners() {
        return {{gx, gy}, {gx+w, gy}, {gx, gy+h}, {gx+w, gy+h}};
    }

    //Get the vector from (x,y) to the closest corner of this collider
    // Vector2 closest_edge_delta(float ox, float oy){
    //     V<float> deltas = {};
    //     deltas.push_back(ox-gx);
    //     deltas.push_back(ox-(gx+w));
    //     deltas.push_back(oy-gy);
    //     deltas.push_back(oy-(gy+h));
    //     int max_i = -1;
    //     int max = std::numeric_limits<float>::min();
    //     for(int i = 0; i < 4; i++){
    //         if(std::abs(deltas[i]) < 2 && std::abs(deltas[i]) > max){
    //             max_i = i;
    //             max = std::abs(deltas[i]);
    //         }
    //     }
    //     if(max_i < 0){
    //         return {0, 0};
    //     }
    //     else if(max_i <= 1){
    //         return {deltas[max_i], 0};
    //     } else {
    //         return {0, deltas[max_i]};
    //     }
    // }

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

    static int initialize() {
        default_meta(_Collider, "Col");
        manual_heap_meta(_Collider);
        meta_set_max_elements(typeid(_Collider), 40);
        return 0;
    }
};
static Typ __Collider = typeid(_Collider);


class _Level : public Component {
    public:
    Texture2D tilemap;
    u_int16_t grid[BLOCKS_Y][BLOCKS_X];

    _Level(std::string level_path, std::string tilemap_path) : 
        tilemap(LoadTexture(tilemap_path.c_str())) 
        {set_grid(level_path);} 

    bool is_inside(int tile_value, float x, float y, float w, float h){
        int bx0 = int(x)/BLOCK_SIZE;
        int bx1 = int(x+w)/BLOCK_SIZE+1;
        int by0 = BLOCKS_Y - (y+h)/BLOCK_SIZE;
        int by1 = BLOCKS_Y - (y)/BLOCK_SIZE + 1;
        for(int i = bx0; i < bx1; i++){
            for(int j = by0; j < by1; j++){
                if(i >= 0 && j >= 0 && grid[j][i]==tile_value){
                    return true;
                }
            }
        }
        return false;
    }

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

    static int initialize() {
        default_meta(_Level, "Lvl");
        return 0;
    }
};
static Typ __Level = typeid(_Level);

class _Player : public Component {
    public:
    Id ground_trigger_col_id;
    Id falling_trigger_col_id;

    _Player(Id ground_trigger_col_id, Id falling_trigger_col_id) : ground_trigger_col_id(ground_trigger_col_id), falling_trigger_col_id(falling_trigger_col_id) {}

    static int initialize() {
        default_meta(_Player, "Pl");
        return 0;
    }
};
static Typ __Player = typeid(_Player);


class _Oscillator : public Component {
    public:
    bool dir;
    int distance;
    float period;

    float time;

    bool forwards; //bool that tells you if it is in the forward or backwards cycle

    _Oscillator(bool dir, int distance, float period) : dir(dir), distance(distance), period(period), time(0), forwards(true) {};
    _Oscillator(_Oscillator* osc) : dir(osc->dir), distance(osc->distance), period(osc->period), time(osc->time), forwards(osc->forwards) {};

    void tick(float time_delta) {
        time += time_delta;
    }
    void reset() {
        time = 0;
    }
    //THIS IS TEMP, WE HAVEN'T ACTUALLY COMPUTED SPEED YET!!
    float get_speed() {
        return (-1 + forwards*2)*(2*distance / period);
    }

    static int initialize() {
        default_meta(_Oscillator, "Osc");
        return 0;
    }
};
static Typ __Oscillator = typeid(_Oscillator);

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

    static int initialize() {
        default_meta(_Animation_player, "Anim");
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
static Typ __Animation_player = typeid(_Animation_player);


class _DebugCollision : public Component {
   public:
   bool shit;
   _DebugCollision() : shit(true) {};
   
   static int initialize() {
      default_meta(_DebugCollision, "debugCollision");
      return 0;
   }
};
static Typ __DebugCollision = typeid(_DebugCollision);


#endif