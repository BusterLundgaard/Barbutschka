#include <system.h>
#include <systems.h>
#include <components.h>
#include "utils.h"
#include <algorithm>

Entity_individual_system sys_velocity{
    "velocity",
    {__Velocity, __Transform},
    [](Ecs_m& em, Id id){
        auto t = em.get_from_entity<_Transform>(id);
        auto v = em.get_from_entity<_Velocity>(id);  
        t->x += v->x*em.fl;
        t->y += v->y*em.fl;
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
    [](Ecs_m& em, Component* el){
        auto t = static_cast<_Transform*>(el);
        t->px = t->x;
        t->py = t->y;
    }
};

Entity_individual_system sys_make_colliders_from_lvl {
    "create_colliders_from_level",
    {__Level},
    __first_frame {
        auto lvl = em.get_from_entity<_Level>(id);
        
        em.add(_Transform(0,0), id);

        for(int i = 0; i < BLOCKS_X; i++){
            for(int j = 0; j < BLOCKS_Y; j++){
                if(lvl->grid[j][i] == 1){
                    em.add(_Collider(i*BLOCK_SIZE, GAME_HEIGTH - (j+1)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, false, false, true), id);
                }
                else if(lvl->grid[j][i] == 4){
                    em.add(_Collider(i*BLOCK_SIZE, GAME_HEIGTH - (j+1)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, false, false, true, true, false), id);
                }
                else if(lvl->grid[j][i] == 5){
                    em.add(_Collider(i*BLOCK_SIZE, GAME_HEIGTH - (j+1)*BLOCK_SIZE, BLOCK_SIZE, BLOCK_SIZE, false, false, true, true, true), id);
                }
            }
        }

        std::cout << "trolololo\n";
    },
    __update {}
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
    [](Ecs_m& em, V<Component*> els){
        auto cs = static_cast_all<_Collider>(els);
        auto lvl = em.get_singleton<_Level>();

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