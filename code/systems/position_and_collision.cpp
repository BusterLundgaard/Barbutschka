#include <system.h>
#include <systems.h>
#include <components.h>
#include "utils.h"
#include <algorithm>
#include "globals.h"
#include <math.h>

Entity_individual_system sys_velocity_non_adjustable{
    "velocity_non_adjustable",
    {__Velocity, __Transform}, {F_Non_adjustable},
    GRAPHIC_LAYER::none,
    __update {
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);  
        t->x += v->x*em.fl;
        t->y += v->y*em.fl;
    }
};


// If these colliders hit the terrain, return the first thing they hit. Otherwise return 0
// This is very poorly optimized, and should be made specific to what direction you actually move in!
V<int> terrain_hit(_Transform* t, V<_Collider*> cols) {
    V<int> hits = {};
    for(_Collider* col : cols){

        int bx0 = int(t->x + col->x)/BLOCK_SIZE;
        int bx1 = int(t->x + col->x + col->w)/BLOCK_SIZE+1;
        int by0 = BLOCKS_Y - (t->y + col->y + col->h)/BLOCK_SIZE;
        int by1 = BLOCKS_Y - (t->y + col->y)/BLOCK_SIZE + 1;

        for(int i = bx0; i < bx1; i++){
            for(int j = by0; j < by1; j++){
                if(i >= 0 && j >= 0 && GRID[j][i] != 0){
                    hits.push_back(GRID[j][i]);
                }
            }
        }
    }
    return hits;
}

//If these colliders hit some solid, return the id of the solid they hit. Otherwise return -1
// This is very poorly optimized, and should be made specific to what direction you actually move in!
V<Id> solid_hit(_Transform* t, V<_Collider*> cols, V<_Collider*> solids){
    V<Id> hits = {};
    for(_Collider* col : cols){
        for(_Collider* sol : solids){
            if(sol->is_inside(t->x + col->x, t->y + col->y, col->w, col->h)){
                hits.push_back(sol->comp_id);
            }
        }
    }
    return hits;
}
V<Id> adjustable_hit(_Transform* t, V<_Collider*> solids, V<_Collider*> adjustables){
    V<Id> hits = {};
    for(_Collider* adj : adjustables){
        for(_Collider* sol : solids){
            if(adj->is_inside(t->x + sol->x, t->y + sol->y, sol->w, sol->h)){
                hits.push_back(adj->comp_id);
            }
        }
    }
    return hits;
}

Entity_individual_system sys_velocity_adjustable{
    "velocity_adjustable",
    {__Velocity, __Transform, __Collider}, {F_Adjustable},
    GRAPHIC_LAYER::none, 
    __update {
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);  
        auto cs = em.get_all_from_entity<_Collider>(id); //CURRENTLY THIS ONLY WORKS IF YOU HAVE ONLY 1 COLLIDER! ( at least for slopes... )

        auto solids_ids = em.get_all_entities_with_flag(F_Solid);
        V<_Collider*> solids = {};
        for(Id solid_id : solids_ids){
            if(em.has_type(solid_id, __Collider)){
                solids.push_back(em.get_from_entity<_Collider>(solid_id));
            }
        }
        
        Vector2 vel_remaining = {std::abs(v->x)*em.fl, std::abs(v->y)*em.fl};
        int dir_x = -1 + (v->x >= 0)*2;
        int dir_y = -1 + (v->y >= 0)*2;

        bool done = false;
        while (!done) {
            done = true;

            if(vel_remaining.x > 0){
                done=false;
                float x_before = t->x;
                t->x += dir_x*std::min(1.0f, vel_remaining.x);
                
                V<int> ter_hits = terrain_hit(t, cs);
                if(exists(ter_hits, 5)){
                    int slope_y = (int(t->y + cs[0]->y)/BLOCK_SIZE)*BLOCK_SIZE;
                    int slope_x = (int(t->x + cs[0]->x)/BLOCK_SIZE)*BLOCK_SIZE;
                    int extra_margin = 0;
                    if(t->x + cs[0]->x > slope_x + 3 && t->y + cs[0]->y < slope_y + 1*(t->x + cs[0]->x + cs[0]->w - slope_x) + extra_margin){
                        if(id==PLAYER_ID){
                            auto player = em.get_from_entity<_Player>(id);
                            if(player->grounded){
                                t->y += dir_x;
                                vel_remaining.y = std::abs(dir_x);
                            }
                        }
                    }
                    ter_hits.erase(std::find(ter_hits.begin(), ter_hits.end(), 5));
                }
                if(solid_hit(t, cs, solids).size() != 0 || ter_hits.size() != 0){
                    t->x = x_before;
                }
                vel_remaining.x -= 1;
            }
            if(vel_remaining.y > 0){
                done=false;
                float y_before = t->y;
                bool stopped = false;
                std::optional<Id> hit_oscillator = {};
                t->y += dir_y*std::min(1.0f, vel_remaining.y);

                V<int> ter_hits = terrain_hit(t, cs);
                V<Id> solid_hits = solid_hit(t, cs, solids);
                if(exists(ter_hits, 5)){
                    int slope_y = ((int(t->y + cs[0]->y+1) - 1)/BLOCK_SIZE)*BLOCK_SIZE;
                    int slope_x = (int(t->x + cs[0]->x + cs[0]->w)/BLOCK_SIZE)*BLOCK_SIZE;
                    if(t->y + cs[0]->y < slope_y + 1.05*(t->x + cs[0]->x + cs[0]->w - slope_x)){
                        t->y = slope_y + 1.05*(t->x + cs[0]->x + cs[0]->w - slope_x) - cs[0]->y;
                        stopped=true;
                    }
                    ter_hits.erase(std::find(ter_hits.begin(), ter_hits.end(), 5));
                    vel_remaining.y = 0;
                }
        
                if(solid_hits.size() != 0){
                    t->y = y_before;
                    stopped=true;
                    hit_oscillator = em.get_first_sibling_of_type(solid_hits, __Oscillator);
                    vel_remaining.y = 0;
                }
                else if(ter_hits.size() != 0){
                    t->y = y_before;
                    stopped=true;
                    vel_remaining.y = 0;
                }
                vel_remaining.y -= 1;
    
                if(id==PLAYER_ID){
                    auto player = em.get_from_entity<_Player>(id);
                    if (!player->grounded && stopped && dir_y == -1){
                        if(hit_oscillator){
                            em.emit_event(EVENT::PLAYER_GROUNDED, Event_data {PLAYER_GROUNDED_data {hit_oscillator}}, 0);
                        } else {
                            em.emit_event(EVENT::PLAYER_GROUNDED, Event_data {PLAYER_GROUNDED_data {{}}}, 0);
                        }
                    } 
                    else if (player->grounded && !stopped && std::abs(t->y - t->py) >= 1 && vel_remaining.y <= 0){
                        em.emit_event(EVENT::PLAYER_UNGROUNDED, Event_data {NO_DATA {}}, 0);
                    }
                }
            }

        }

    }
};

Entity_individual_system sys_velocity_solid{
    "velocity_solid",
    {__Collider, __Velocity, __Transform}, {F_Solid},
    GRAPHIC_LAYER::none,
    __update {
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);  
        auto cs = em.get_all_from_entity<_Collider>(id);

        auto adjustable_ids = em.get_all_entities_with_flag(F_Adjustable);
        V<_Collider*> adjustable = {};
        for(Id adjustable_id : adjustable_ids){
            if(em.has_type(adjustable_id, __Collider)){
                adjustable.push_back(em.get_from_entity<_Collider>(adjustable_id));
            }
        }
        
        Vector2 vel_remaining = {std::abs(v->x)*em.fl, std::abs(v->y)*em.fl};
        int dir_x = -1 + (v->x >= 0)*2;
        int dir_y = -1 + (v->y >= 0)*2;
        bool done = false;
        while(!done){
            done=true;

            if(vel_remaining.x > 0){
                t->x += dir_x;
                auto hits = adjustable_hit(t, cs, adjustable);
                for(Id hit : hits){
                    auto v_hit = em.get_sibling<_Velocity>(hit);
                    v_hit->x = (dir_x == 1) ? std::max(1.0f/em.fl, v_hit->x + 1.0f/em.fl) : std::min(-1.0f/em.fl, v_hit->x - 1.0f/em.fl);
                }
                vel_remaining.x--;
                done=false;
            }
            if(vel_remaining.y > 0){
                t->y += dir_y;
                auto hits = adjustable_hit(t, cs, adjustable);
                for(Id hit : hits){
                    auto v_hit = em.get_sibling<_Velocity>(hit);
                    v_hit->y = (dir_y == 1) ? std::max(1.0f/em.fl, v_hit->y + 1.0f/em.fl) : std::min(-1.0f/em.fl, v_hit->y - 1.0f/em.fl);
                }
                vel_remaining.y--;
                done=false;
            }

        }
    }
};

Entity_individual_system sys_update_collider_global_pos{
    "update_collider_global_pos", 
    {__Collider, __Transform}, {},
    GRAPHIC_LAYER::none,
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
    GRAPHIC_LAYER::none,
    [](Ecs_m& em, Component* el){
        auto t = static_cast<_Transform*>(el);
        t->px = t->x;
        t->py = t->y;
    }
};

Entity_individual_system sys_add_movement_from_parent{   
    "add_movement_from_parent",
    {__ParentLink, __Transform}, {},
    GRAPHIC_LAYER::none, 
    __update {
        auto t = em.get_from_entity<_Transform>(id);
        auto pl = em.get_from_entity<_ParentLink>(id);
        auto par_t = em.get_from_entity<_Transform>(pl->parent_entity_id);

        t->x += par_t->x - par_t->px;
        t->y += par_t->y - par_t->py;
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
    GRAPHIC_LAYER::none,
    [](Ecs_m& em, V<Component*> els){
        auto cs = static_cast_all<_Collider>(els);
        auto lvl = em.get_singleton<_LevelBuilder>();

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
                if(cs[z]->is_slope){continue;}
                if(cs[i]->is_inside(cs[z]->gx, cs[z]->gy, cs[z]->w, cs[z]->h)){
                    cs[z]->hit.insert(cs[i]->comp_id);
                    cs[i]->hit.insert(cs[z]->comp_id);
                }
            }

            //Static terrain
            if(!cs[i]->hits_terrain){continue;}

            if(lvl->is_inside(1, cs[i]->gx, cs[i]->gy, cs[i]->w, cs[i]->h)){
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
            auto corners = col->corners();

            std::vector<_Collider*> solids;
            for(auto hit : col->hit){
                _Collider* comp = em.get_from_comp<_Collider>(hit);
                if(comp->solid){solids.push_back(comp);}
            }

            for(auto solid : solids){
                if(!solid->is_slope){
                    V<Vector2> gaps = {};
                    if(col->gx + col->w > solid->gx && col->gx < solid->gx){
                        gaps.push_back({solid->gx - col->gx - col->w, 0});
                    }
                    if(col->gx < solid->gx + solid->w && col->gx > solid->gx){
                        gaps.push_back({solid->gx+solid->w - col->gx, 0});
                    }
                    if(col->gy + col->h > solid->gy && col->gy < solid->gy){
                        gaps.push_back({0, solid->gy - col->gy - col->h});
                    }
                    if(col->gy < solid->gy + solid->h && col->gy > solid->gy){
                        gaps.push_back({0, solid->gy+solid->h - col->gy});
                    }
                    std::sort(gaps.begin(), gaps.end(), [](const Vector2& a, const Vector2& b){      
                        return std::abs(b.x) + std::abs(b.y) > std::abs(a.x) + std::abs(a.y);
                    });
                    int i = 0; 
                    while(solid->is_inside(col->gx, col->gy, col->w, col->h) && i < gaps.size()){
                        t->x += gaps[i].x;
                        t->y += gaps[i].y;
                        update_collider_global_pos(em, t);
                        i++;
                    }
                } else {
                    auto v = em.get_sibling<_Velocity>(col->comp_id);
                    if(solid->slope_dir && 
                       col->gx + col->w < solid->gx + BLOCK_SIZE + 0.5 && 
                       col->gy < solid->gy + (col->gx + col->w - solid->gx) + 1){

                        t->y -= 1.1f*(col->gy - (solid->gy + (col->gx + col->w - solid->gx)));
                        v->y = -25;
                    } 
                    else if(
                        !solid->slope_dir && 
                        col->gx > solid->gx - 0.5 && 
                        col->gy < solid->gy + (solid->gx + solid->w - col->gx) + 1){
                        t->y -= 1.1f*(col->gy - (solid->gy + (solid->gx + solid->w - col->gx)));
                        v->y = -25;
                    }
                }
                
                //V<Vector2> deltas = {};
                // for(Vector2 corner : corners){
                //     deltas.push_back(solid->closest_edge_delta(corner.x,corner.y));
                // }
                // int min_i = -1;
                // int min = std::numeric_limits<float>::max();
                // for(int i = 0; i < 4; i++){
                //     float size = std::abs(deltas[i].x) + std::abs(deltas[i].y);
                //     if(size != 0 && size < min){
                //         min_i = i; 
                //         min = size;
                //     }
                // }
                // if(min_i != -1){
                //     t->x -= deltas[min_i].x;
                //     t->y -= deltas[min_i].y;
                // } 
            }
            // for(auto solid : solids){
            //     _Transform* st = em.get_sibling<_Transform>(solid->comp_id);
            //     float delta_x = st->x - st->px;
            //     float delta_y = st->y - st->py;
            //     if(!solid->is_inside(px + delta_x, col->gy, col->w, col->h)){
            //         if(t->x - t->px - delta_x > 0) { //to the left before
            //             t->x -= (col->gx + col->w) - solid->gx; 
            //         } else { //to the right before
            //             t->x -= col->gx - (solid->gx + solid->w);
            //         }
            //     }
            //     if(!solid->is_inside(col->gx, py + delta_y, col->w, col->h)){
            //         if(t->y - t->py - delta_y > 0) { //below before
            //             t->y -= (col->gy + col->h) - solid->gy; 
            //         } else { // above before
            //             t->y -= col->gy - (solid->gy + solid->h);
            //         }   
            //     }

            //     update_collider_global_pos(em, t);
            // }
        }
    }
};