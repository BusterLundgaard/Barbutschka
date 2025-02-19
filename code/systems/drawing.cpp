#include <system.h>
#include <systems.h>
#include <components.h>
#include "graphic_layers.h"
#include "events.h"
#include "globals.h"

Entity_individual_system sys_draw_sprite{
    "draw_sprite",
    {__Sprite, __Transform}, {},
    GRAPHIC_LAYER::dynamic,
    [](Id id){
        auto spr = em.get_from_entity<_Sprite>(id);
        auto t = em.get_from_entity<_Transform>(id);
        DrawTexture(spr->tex, t->x, GAME_HEIGTH - t->y - spr->tex.height, WHITE);
    }
};

Entity_individual_system sys_draw_world {
    "draw_world",
    {__LevelBuilder}, {},
    GRAPHIC_LAYER::static_level, 
    __first_frame {}, 
    __system_events {
        {EVENT::NEW_SCREEN, [](Id id, Event_data data){
            NEW_SCREEN_data new_screen_data = std::get<NEW_SCREEN_data>(data); // We will actually use this data later ...
            auto lvl_builder = em.get_singleton<_LevelBuilder>();

            int tilenum = 0;
            for(int i = 0; i < BLOCKS_Y; i++){
                for(int j = 0; j < BLOCKS_X; j++){
                    tilenum = GRID[i][j];
                    if(tilenum == 0){continue;}
                    DrawTextureRec(
                        lvl_builder->tilemap, 
                        Rectangle{float(BLOCK_SIZE)*(tilenum-1), 0, BLOCK_SIZE, BLOCK_SIZE},
                        Vector2{float(BLOCK_SIZE)*j, float(BLOCK_SIZE)*i},
                        WHITE
                    );
                }
            }
        }}
    },
    __update {}
};


Entity_individual_system sys_draw_animation{
    "draw_animation", 
    {__Animation_player, __Transform}, {},
    GRAPHIC_LAYER::dynamic,
    __update {
        auto t = em.get_from_entity<_Transform>(id);
        auto anim = em.get_from_entity<_Animation_player>(id);

        anim->time += em.fl;
        if(anim->time > anim->anims.at(anim->current_anim).frame_speed){
            anim->time=0;
            int amount_of_frames = anim->anims.at(anim->current_anim).spritesheet.width/anim->sprite_width;
            if(anim->anims.at(anim->current_anim).loops){
                anim->frame = (anim->frame + 1) % amount_of_frames;
            } else {
                anim->frame = std::min(amount_of_frames-1, anim->frame + 1);
            }
           
        }

        DrawTextureRec(
            anim->anims.at(anim->current_anim).spritesheet, 
            Rectangle{
                float(anim->sprite_width*anim->frame), 0, 
                float(anim->sprite_width), float(anim->sprite_height)},
            Vector2{t->x + anim->offset_x, GAME_HEIGTH - t->y - anim->offset_y - anim->sprite_height}, 
            WHITE
        );
    }
};


Component_individual_system sys_DEBUG_draw_hit_collider(
    "DEBUG_draw_hit_collider",
    __Collider, 
    GRAPHIC_LAYER::dynamic,
    [](Component* el){
        auto col = static_cast<_Collider*>(el);

        if(!col->is_slope){
            DrawLine(col->gx,              GAME_HEIGTH - col->gy,            col->gx + col->w,      GAME_HEIGTH - (col->gy),          BLUE);
            DrawLine(col->gx,              GAME_HEIGTH - col->gy,            col->gx,               GAME_HEIGTH - (col->gy + col->h), BLUE);
            DrawLine(col->gx,              GAME_HEIGTH - (col->gy + col->h), col->gx + col->w,      GAME_HEIGTH - (col->gy + col->h), BLUE);
            DrawLine(col->gx + col->w + 1, GAME_HEIGTH - col->gy,            col->gx + col->w + 1,  GAME_HEIGTH - (col->gy + col->h), BLUE);
        } else if(col->slope_dir) {
            DrawLine(col->gx, GAME_HEIGTH - col->gy, col->gx + col->w, GAME_HEIGTH - col->gy, BLUE); 
            DrawLine(col->gx + col->w, GAME_HEIGTH - col->gy, col->gx + col->w, GAME_HEIGTH - (col->gy + col->h), BLUE);
            DrawLine(col->gx, GAME_HEIGTH - col->gy, col->gx + col->w, GAME_HEIGTH - (col->gy + col->h), BLUE);
        } else {
            DrawLine(col->gx, GAME_HEIGTH - col->gy, col->gx + col->w, GAME_HEIGTH - col->gy, BLUE); 
            DrawLine(col->gx, GAME_HEIGTH - col->gy, col->gx, GAME_HEIGTH - (col->gy + col->h), BLUE);
            DrawLine(col->gx, GAME_HEIGTH - (col->gy + col->h), col->gx + col->w, GAME_HEIGTH - col->gy, BLUE);
        }

        //Draw hits:
        for (int16_t hit_id : col->hit) {
            auto hit_col = em.get_from_comp<_Collider>(hit_id);
            draw_arrow(col->gx, GAME_HEIGTH - col->gy, hit_col->gx, GAME_HEIGTH - hit_col->gy, RED);
        }
        if (col->hit_terrain) {
            DrawRectangle(0, 0, 10, 10, BLUE);
        }
    }
);