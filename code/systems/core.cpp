#include "../components.h"
#include "../ecs_manager.h"
#include "../systems.h"

Entity_System_Individual sys_draw_sprite(
    "draw_sprite_system",
    {typeid(_Sprite), typeid(_Transform)},
    entity_individual_signature {
        param(_Sprite, s)(comps[0][0]);
        param(_Transform, t)(comps[1][0]);
        DrawTexture(s->tex, t->x, t->y, WHITE);
    }
);

Entity_System_Individual sys_velocity(
    "velocity_system",
    {typeid(_Velocity), typeid(_Transform)},
    entity_individual_signature {
        param(_Velocity, v)(comps[0][0]);
        param(_Transform, t)(comps[1][0]);
        t->x += v->x;
        t->y += v->y;
    }
);

// Collision
Entity_System_Individual sys_update_collider_global_pos(
    "update_collider_global_pos_system)",
    {typeid(_HitCollider), typeid(_Transform)},
    entity_individual_signature {
        param(_Transform, t)(comps[1][0]);
        std::vector<Component*> cols = comps[0];
        for(auto col : cols){
            _HitCollider* as_col = static_cast<_HitCollider*>(col);
            as_col->gx = t->x + as_col->x;
            as_col->gy = t->y + as_col->y;
        }
    }
);

All_Component_System sys_find_collider_hits(
    "find_collider_hits_system",
    {typeid(_HitCollider)},
    all_component_signature {

        auto cs = std::vector<_HitCollider*>(comps[0].size());
        for(int i = 0; i < comps[0].size(); i++){cs[i] = static_cast<_HitCollider*>(comps[0][i]);}
        std::sort(cs.begin(), cs.end(), [](_HitCollider* &a, _HitCollider* &b) {  
            return a->gx - b->gx;
        });

        //Determine what each one collides with
        for(int i = 0; i < cs.size(); i++){
            cs[i]->p_hit = cs[i]->hit;
            cs[i]->hit = {};
            std::vector<int> potential = {};
            int j = i-1;
            while(j >= 0 && cs[j]->gx + cs[j]->w > cs[i]->gx){
                potential.push_back(j);
                j--;                
            }
            j = i+1;
            while(j <= cs.size() && cs[j]->gx < cs[i]->gx + cs[i]->w){
                potential.push_back(j);
                j++;                
            }
            for(int z : potential){
                if(cs[z]->gy < cs[i]->gy + cs[i]->h ||  
                   cs[z]->gy + cs[z]->h > cs[i]->gy) {
                    cs[z]->hit.insert(em->get_entity(cs[i]->component_id, typeid(_HitCollider)));
                   }
            }
        }       
    }
);

Entity_System_Individual sys_DEBUG_draw_hit_collider(
    "DEBUG_draw_hit_collider",
    {typeid(_HitCollider)},
    entity_individual_signature{
        param(_HitCollider, col)(comps[0][0]);
        DrawLine(col->gx,        col->gy,        col->gx + col->w,  col->gy,        GREEN);
        DrawLine(col->gx,        col->gy,        col->gx,           col->gy+col->h, GREEN);
        DrawLine(col->gx,        col->gy+col->h, col->gx + col->w,  col->gy+col->h, GREEN);
        DrawLine(col->gx+col->w, col->gy,        col->gx + col->w,  col->gy+col->h, GREEN);
    }
);

//World:
Entity_System_Individual sys_draw_world(
    "draw_world",
    {typeid(_Level)},
    entity_individual_signature{
        param(_Level, lvl)(comps[0][0]);

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

        return 0;
    }
);
