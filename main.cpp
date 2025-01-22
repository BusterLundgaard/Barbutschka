#include "code/defs.h"
#include "code/input.h"
#include "code/ecs_m.h"
#include "code/systems.h"
#include "code/components.h"
#include "code/initialize_components.h"

int main(){
    initialize_components();

    Ecs_m em({
        &sys_player_movement,
        
        &sys_oscillator_movement,

        &sys_velocity, 
        &sys_update_collider_global_pos, 
        &sys_collision,
        
        &sys_draw_sprite, 
        &sys_draw_animation,
        &sys_draw_world,
        &sys_DEBUG_draw_hit_collider, 
        &sys_set_prev_pos
        });

    // Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(GAME_WIDTH*4, GAME_HEIGTH*4, "Bartbushka");
	SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGTH);

    int entity_id = 1;

    em.add(_Transform(10, 0), entity_id);
    em.add(_Collider(0, 0, 68, 19, false, false, true), entity_id);
    entity_id++;

    // Collider 2
    em.add(_Transform(150, 0), entity_id);
    em.add(_Collider(0, 0, 123, 19, false, false, true), entity_id);
    entity_id++;

    // player
    em.add(_Transform(61.060257, 30), entity_id);
    em.add(_Velocity(0,0), entity_id);
    em.add(_Collider(0, 0, 15, 27, true, true, false), entity_id);
    _Collider ground_trigger(1, 0, 10, 3, false, false, false);
    em.add(ground_trigger, entity_id);
    em.add(_Player(ground_trigger.comp_id), entity_id);
        em.add(
        _Animation_player(
            {{"idle", {LoadTexture("../assets/tmp/Idle.png"), 1.0/5.0, true}}, 
             {"walk", {LoadTexture("../assets/tmp/Walk.png"), 1.0/15.0, true}},
             {"jump_start", {LoadTexture("../assets/tmp/JumpStart.png"), 1.0/4.0, false}},
             {"jump_fall", {LoadTexture("../assets/tmp/JumpFall.png"), 1.0/5.0, true}}
            }, 1.0/5.0, 20, 32, -4, 0, "idle"), 
        entity_id);
    named_entities.insert({entity_id, "Player"});
    entity_id++;

    // moving platform
    em.add(_Transform(90, 40), entity_id);
    em.add(_Collider(0, 0, 60, 15, false, false, true), entity_id);
    em.add(_Velocity(0,0), entity_id);
    em.add(_Oscillator(false, 70, 1.0f), entity_id);

    em.update();
    em.print_table();

    bool paused = false;
    int speed_mod = 0;

    register_key(KEY_K);
    register_key(KEY_J);
    register_key(KEY_L);

    while(!WindowShouldClose()){
        float scale = std::min(
            (float)GetScreenWidth()/GAME_WIDTH, 
            (float)GetScreenHeight()/GAME_HEIGTH
        );

        if(Key_Press(KEY_K)){
            if(!paused){
                em.print_table();
            }
            paused = !paused;
        }
        if(Key_Press(KEY_J)){
            speed_mod--;
            std::cout << "Decreased speed to " << pow(2, speed_mod) << std::endl;
            em.set_time_scale(pow(2, speed_mod));
        }
        if(Key_Press(KEY_L)){
            speed_mod++;
            std::cout << "Increased speed to " << pow(2, speed_mod) << std::endl;
            em.set_time_scale(pow(2, speed_mod));
        }

        if(!paused){
            BeginTextureMode(target);
            ClearBackground(WHITE);
            em.update();
            EndTextureMode();
        }
        
        BeginDrawing();
        ClearBackground(WHITE);
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
        new_frame();
        EndDrawing();
    }
    return 0;
}