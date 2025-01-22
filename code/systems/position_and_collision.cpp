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