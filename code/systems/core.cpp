#include "../components.h"
#include "../ecs_manager.h"
#include "../systems.h"

Entity_system_individual sys_draw_sprite(
    "draw_sprite_system",
    {__Sprite, __Transform},
    entity_individual_signature {
        auto s = em.get<_Sprite>(comps[0][0]);
        auto t = em.get<_Transform>(comps[1][0]);

        DrawTexture(s.tex, t.x, t.y, WHITE);
    }
);

Entity_system_individual sys_velocity(
    "velocity_system",
    {__Velocity, __Transform},
    entity_individual_signature {
        auto v = em.get<_Velocity>(comps[0][0]);
        auto t = em.get<_Transform>(comps[1][0]);

        t.x += v.x;
        t.y += v.y;
    }
);

// // Collision
Entity_system_individual sys_update_collider_global_pos(
    "update_collider_global_pos_system)",
    {__HitCollider, __Transform},
    entity_individual_signature {
        V<_HitCollider&> cols = em.get_all<_HitCollider>(comps[0]);
        _Transform& t = em.get<_Transform>(comps[1][0]);

        for(_HitCollider& col : cols){
            col.gx = t.x + col.x;
            col.gy = t.y + col.y;
        }
    }
);

Component_system_all sys_find_collider_hits(
    "find_collider_hits_system",
    {__HitCollider},
    components_all_signature {

        V<_HitCollider&> cs = em.get_all<_HitCollider>(comps);
        _Level& lvl = em.get_singleton<_Level>();

        std::sort(cs.begin(), cs.end(), [](_HitCollider &a, _HitCollider &b) {  
            return a.gx < b.gx;
        });

        //Determine hits
        for(int i = 0; i < cs.size(); i++){
            cs[i].p_hit = cs[i].hit;
            cs[i].hit = {};
            cs[i].p_hit_terrain=cs[i].hit_terrain;
            cs[i].hit_terrain=false;

            //Other colliders
            std::vector<int> potential = {};
            int j = i-1;
            while(j >= 0 && cs[j].gx + cs[j].w > cs[i].gx){
                potential.push_back(j);
                j--;                
            }
            j = i+1;
            while(j < cs.size() && cs[j].gx < cs[i].gx + cs[i].w){
                potential.push_back(j);
                j++;                
            }
            for(int z : potential){
                if(cs[z].gy < cs[i].gy + cs[i].h &&  
                   cs[z].gy + cs[z].h > cs[i].gy) {
                    cs[z].hit.insert(cs[i].component_id);
                    cs[i].hit.insert(cs[z].component_id);
                   }
            }

            //Static terrain
            bool hits = cs[i].hits_terrain;
            if(!hits){continue;}
            if(!em.singleton_exists(typeid(_Level))){continue;}
            if(lvl.is_inside(cs[i].gx, cs[i].gy, cs[i].w, cs[i].h)){
                cs[i].hit_terrain=true;
            }
        }       
    
        //Adjust positions:
        auto needs_adjustment = filter<_HitCollider&>(cs, [](_HitCollider& h){
            return h.adjustable && h.hit.size() > 0;
        });

        for(_HitCollider& col : needs_adjustment){
            _Transform& t = em.get_sibling<_Transform>(col.component_id);
            float px = t.px + col.x;
            float py = t.py + col.y;

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
            std::vector<_HitCollider&> solids;
            for(auto hit : col.hit){
                _HitCollider& comp = em.get<_HitCollider>(hit);
                if(comp.solid){solids.push_back(comp);}
            }
            for(auto solid : solids){
                _Transform& st = em.get_sibling<_Transform>(solid.component_id);
                float delta_x = st.x - st.px;
                float delta_y = st.y - st.py;
                if(solid.is_inside(px - delta_x, col.gy, col.w, col.h)){
                    std::cout << "hit on x axis! \n";
                    if(delta_x < 0) { //to the left before
                        t.x -= col.gx + col.w - solid.gx;
                    } else { //to the right before
                        t.x += solid.gx + solid.w - col.gx;
                    }
                    col.gx = t.x + col.x;
                }
                if(solid.is_inside(col.gx, py - delta_y, col.w, col.h)){
                    std::cout << "hit on y axis! \n";
                    if(delta_y < 0) { //below before
                        t.y -= col.gy + col.h - solid.gy;  
                    } else { // above before
                        t.y += solid.gy + solid.h - col.gy;
                    }   
                    col.gy = t.y + col.y; 
                }
            }
        }
    }
);

Component_system_individual sys_DEBUG_draw_hit_collider(
    "DEBUG_draw_hit_collider",
    __HitCollider,
    components_individual_signature{
        auto col = em.get<_HitCollider>(comp);

        DrawLine(col.gx,       col.gy,        col.gx + col.w,  col.gy,       GREEN);
        DrawLine(col.gx,       col.gy,        col.gx,          col.gy+col.h, GREEN);
        DrawLine(col.gx,       col.gy+col.h,  col.gx + col.w,  col.gy+col.h, GREEN);
        DrawLine(col.gx+col.w, col.gy,        col.gx + col.w,  col.gy+col.h, GREEN);

        //Draw hits:
        for(int16_t hit_id : col.hit){
            auto hit_col = em.get<_HitCollider>(hit_id);
            draw_arrow(col.gx, col.gy, hit_col.gx, hit_col.gy, RED);            
        }
        if(col.hit_terrain){
            DrawRectangle(0,0,10,10, BLUE);
        }
    }
);

// //World:
Component_system_individual sys_draw_world(
    "draw_world",
    __Level, 
    components_individual_signature{
        auto lvl = em.get<_Level>(comp);

        int tilenum = 0;
        for(int i = 0; i < BLOCKS_Y; i++){
            for(int j = 0; j < BLOCKS_X; j++){
                tilenum = lvl.grid[i][j];
                if(tilenum == 0){continue;}
                DrawTextureRec(
                    lvl.tilemap, 
                    Rectangle{float(BLOCK_SIZE)*(tilenum-1), 0, BLOCK_SIZE, BLOCK_SIZE},
                    Vector2{float(BLOCK_SIZE)*j,float(BLOCK_SIZE)*i},
                    WHITE
                );
            }
        }
    }
);

Component_system_individual sys_set_prev_pos(
    "set_prev_pos",
    __Transform,
    components_individual_signature{
        auto t = em.get<_Transform>(comp);

        t.px=t.x;
        t.py=t.y;
    }
);

Entity_system_individual sys_debug_move(
    "debug_move",
    {__Moveable, __Velocity},
    entity_individual_signature{
        _Velocity& v = em.get<_Velocity>(comps[1][0]);
        
        if (IsKeyDown(KEY_RIGHT)) {
            v.x = 5.0f; 
            v.y = 0;
        }
        if (IsKeyReleased(KEY_RIGHT)){v.x=0; v.y=0;}
        
        if (IsKeyDown(KEY_LEFT))  {
            v.x = -5.0f;
            v.y = 0;
        }
        if (IsKeyReleased(KEY_LEFT)){v.x=0; v.y=0;}
        
        if (IsKeyDown(KEY_UP))    {
            v.y = -5.0f;
            v.x = 0;
        }
        if (IsKeyReleased(KEY_UP)){v.x=0; v.y=0;}
        
        if (IsKeyDown(KEY_DOWN))  {
            v.y = 5.0f;
            v.x = 0;
        }
        if (IsKeyReleased(KEY_DOWN)){v.x=0; v.y=0;}
    }
);


