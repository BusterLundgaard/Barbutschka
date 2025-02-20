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
V<_Collider*> solid_hit(_Transform* t, V<_Collider*> cols, V<_Collider*> solids){
    V<_Collider*> hits = {};
    for(_Collider* col : cols){
        for(_Collider* sol : solids){
            if(sol->is_inside(t->x + col->x, t->y + col->y, col->w, col->h)){
                hits.push_back(sol);
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

std::optional<_Collider*> slope_hit(_Transform* t, _Collider* c0, V<_Collider*>& solid_hits){
    auto slope_col = std::find_if(solid_hits.begin(), solid_hits.end(), [](_Collider* sol){return sol->is_slope;});
    if(slope_col != solid_hits.end()){
        solid_hits.erase(slope_col);
        if(t->y + c0->y < (*slope_col)->gy + ((*slope_col)->h / (*slope_col)->w)*(t->x + c0->x + c0->w - (*slope_col)->gx)){ //only covers one direction
            return std::optional<_Collider*>{*slope_col};
        }
        return std::nullopt;
    }
    return std::nullopt;
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

        // Prevent from moving into collisions in the first frame by moving one pixel at a time:
        bool done = false;
        while (!done) {
            done = true;

            if(vel_remaining.x > 0){
                done=false;
                float x_before = t->x;
                t->x += dir_x*std::min(1.0f, vel_remaining.x);
                
                V<int> ter_hits = terrain_hit(t, cs);
                V<_Collider*> solid_hits = solid_hit(t, cs, solids);

                if(auto slope_col = slope_hit(t, cs[0], solid_hits)){
                    t->y += dir_x;
                    vel_remaining.y = std::abs(dir_x);
                }
                if(solid_hits.size() != 0 || ter_hits.size() != 0){
                    t->x = x_before;
                }
                vel_remaining.x -= 1;
            }
            if(vel_remaining.y > 0){
                done=false;
                float y_before = t->y;
                t->y += dir_y*std::min(1.0f, vel_remaining.y);

                V<int> ter_hits = terrain_hit(t, cs);
                V<_Collider*> solid_hits = solid_hit(t, cs, solids);
                if(auto slope_col = slope_hit(t, cs[0], solid_hits) || solid_hits.size() != 0 || ter_hits.size() != 0){
                    t->y = y_before;
                    vel_remaining.y = 0;
                }

                vel_remaining.y -= 1;
            }
        }

        //Check if somehow still inside collision, respawn to last safe position:
        {
        V<int> ter_hits = terrain_hit(t, cs);         
        V<_Collider*> solid_hits = solid_hit(t, cs, solids);
        if(auto slope_col = slope_hit(t, cs[0], solid_hits) || ter_hits.size() != 0 || solid_hits.size() != 0){
            t->x = t->safe_x;
            t->y = t->safe_y;
        } else {
            t->safe_x = t->x;
            t->safe_y = t->y;
        }
        }

        //Check if grounded:
        if(id == PLAYER_ID){
            t->y -= 1;
            auto player = em.get_from_entity<_Player>(id);

            // Grounded
            if(!player->grounded && dir_y == -1){ 
                V<int> ter_hits = terrain_hit(t, cs);                
                V<_Collider*> solid_hits = solid_hit(t, cs, solids);
                auto slope_col = slope_hit(t, cs[0], solid_hits);
                if(slope_col.has_value() || solid_hits.size() != 0 || ter_hits.size() != 0){
                    auto oscillator_it = std::find_if(solid_hits.begin(), solid_hits.end(), [](_Collider* sol){ 
                        return em.has_type(sol->entity_id, __Oscillator);
                    });
                    auto hit_slope = slope_col.has_value() ? std::optional<Id>((*slope_col)->comp_id) : std::nullopt;
                    auto hit_oscillator = oscillator_it == solid_hits.end() ? std::nullopt : std::optional<Id>((*oscillator_it)->comp_id);

                    em.emit_event(EVENT::PLAYER_GROUNDED, Event_data {PLAYER_GROUNDED_data {hit_oscillator, hit_slope}}, 0);
                }
            } 
            // Ungrounded
            else if(player->grounded && (std::abs(t->y + 1 - t->py) >= 0.1 || std::abs(t->x - t->px) >= 0.1)){
                V<int> ter_hits = terrain_hit(t, cs);
                V<_Collider*> solid_hits = solid_hit(t, cs, solids);
                auto slope_col = slope_hit(t, cs[0], solid_hits);
                if(!slope_col.has_value() && solid_hits.size() == 0 && ter_hits.size() == 0){
                    em.emit_event(EVENT::PLAYER_UNGROUNDED, Event_data {NO_DATA {}}, 0);
                }
            }
            
            //Left slope
            if(player->slope_angle.has_value()){
                V<_Collider*> solid_hits = solid_hit(t, cs, solids);
                auto slope_col = slope_hit(t, cs[0], solid_hits);
                if(!slope_col.has_value()){
                    em.emit_event(EVENT::PLAYER_LEFT_SLOPE, Event_data {NO_DATA {}}, 0);
                }
            } 
            // Entered slope
            else {
                V<_Collider*> solid_hits = solid_hit(t, cs, solids);
                auto slope_col = slope_hit(t, cs[0], solid_hits);
                if(slope_col.has_value()){
                    auto hit_slope = slope_col.has_value() ? std::optional<Id>((*slope_col)->comp_id) : std::nullopt;
                    em.emit_event(EVENT::PLAYER_ENTERED_SLOPE, Event_data {PLAYER_ENTERED_SLOPE_data {(*slope_col)->comp_id}}, 0);
                }
            }

            t->y += 1;
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
                    //it's problematic that we change the *velocity* of the hit object here...
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
    [](Id id){
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
    [](Component* el){
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
    [](V<Component*> els){
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
                
            }

        }
    }
};