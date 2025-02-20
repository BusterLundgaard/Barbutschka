#include "code/defs.h"
#include "code/input.h"
#include "code/ecs_m.h"
#include "code/systems.h"
#include "code/components.h"
#include "code/initialize_components.h"
#include "code/globals.h"
#include "code/graphic_layers.h"

int main(){
    initialize_components();

    em = Ecs_m({
        &sys_player_movement,
        &debug_collision,

        &sys_oscillator_movement,

        &sys_velocity_solid,
        &sys_velocity_adjustable,
        &sys_velocity_non_adjustable,
        //&sys_make_colliders_from_lvl,
        &sys_update_collider_global_pos,
        //&sys_collision,

        &sys_DEBUG_draw_hit_collider,
        &sys_draw_world,
        &sys_draw_sprite,
        &sys_draw_animation,
        &sys_add_movement_from_parent,
        &sys_set_prev_pos
        });

    // Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(GAME_WIDTH*4, GAME_HEIGTH*4, "Bartbushka");
	SetTargetFPS(60);
    initialize_graphic_layers();
    
    int entity_id = 1;
    // player
    PLAYER_ID=entity_id;
    ROOM_X = 1;
    ROOM_Y = 1;
    em.add(_Transform(61.060257, 100), entity_id);
    em.add(_Velocity(0,0), entity_id);
    em.add(_Collider(0, 0, 12, 18, true, true, false), entity_id);
    em.add(_Player(), entity_id);
    em.add(
        _Animation_player(
            {{"idle", {LoadTexture("../assets/tmp/Attempt2/front_view.png"), 1.0/2.0, true, 13, 20, 3, 0}},
             {"walk", {LoadTexture("../assets/tmp/Attempt2/walk.png"), 1.0/15.0, true, 10, 20, 5, 0}},
             {"jump_prep", {LoadTexture("../assets/tmp/Attempt2/jump_prep.png"), 1.0/15.0, false, 11, 22, 0, 0}},
             {"jump_rise", {LoadTexture("../assets/tmp/Attempt2/jump_rise.png"), 1.0/15.0, false, 11, 22, 0, 0}},
             {"fall", {LoadTexture("../assets/tmp/Attempt2/fall.png"), 1.0/15.0, true, 11, 22, 0, 0}},
             {"land", {LoadTexture("../assets/tmp/Attempt2/landing.png"), 1.0/2.0, false, 11, 22, 0, 0}},
             {"slap", {LoadTexture("../assets/tmp/Attempt2/slap.png"), 1.0/5.0, false, 13, 20, 0, 0}},
             {"attack", {LoadTexture("../assets/tmp/Attempt2/kitchen_roll_attack.png"), 1.0/5.0, false, 20, 20, 0, 0}},
             {"crouch", {LoadTexture("../assets/tmp/Attempt2/shield.png"), 1.0/1.0, false, 18, 20, 0, 0}},
            }, 1.0/5.0, 20, 32, -4, 0, "idle"),
        entity_id);
    em.add_flag(F_Adjustable, entity_id);
    named_entities.insert({entity_id, "Player"});
    entity_id++;

    //Slope 1:
    em.add(_Transform(5*16, 16), entity_id);
    em.add(_Collider(0, 0, 3*16, 16, false, false, true, true, 1), entity_id);
    em.add_flag(F_Solid, entity_id);
    entity_id++;

    //Slope 2:
    em.add(_Transform(10*16, 2*16), entity_id);
    em.add(_Collider(0, 0, 3*16, 2*16, false, false, true, true, 1), entity_id);
    em.add_flag(F_Solid, entity_id);
    entity_id++;

    //Some random sprite:
    em.add(_Transform(40, 50), entity_id);
    em.add(_Sprite("../assets/tmp/Attempt2/shield.png", true), entity_id);
    entity_id++;

    // // // Collider 2
    // em.add(_Transform(150, 30), entity_id);
    // em.add(_Collider(0, 0, 200, 20, false, false, true), entity_id);
    // entity_id++;

    // Debug collision
    // em.add(_Transform(0, 50), entity_id);
    // em.add(_Collider(0, 0, 20, 20, false, true, false), entity_id);
    // em.add(_Velocity(0,0), entity_id);
    // em.add(_DebugCollision(), entity_id);
    // em.add_flag(F_Debug_collision | F_Adjustable, entity_id);



    //level
    em.add(_LevelBuilder("../assets/testing_slopes.csv", "../assets/basic_tilemap.png"), entity_id);

    //moving platform
    // em.add(_Transform(80, 32), entity_id);
    // em.add(_Collider(0, 0, 30, 10, false, false, true), entity_id);
    // em.add(_Velocity(0,0), entity_id);
    // em.add(_Oscillator(false, 40, 2.0f), entity_id);
    // em.add_flag(F_Solid, entity_id);

    em.update();
    em.update();
    em.emit_event(EVENT::NEW_SCREEN, Event_data {NEW_SCREEN_data {0, 0}}, 1);
    em.print_table();

    bool paused = false;
    int speed_mod = 0;

    register_key(KEY_K);
    register_key(KEY_J);
    register_key(KEY_L);
    register_key(KEY_Z);
    register_key(KEY_LEFT);
    register_key(KEY_RIGHT);
    register_key(KEY_DOWN);
    
    RenderTexture2D basic1 = LoadRenderTexture(GAME_WIDTH, GAME_HEIGTH);
    RenderTexture2D basic2 = LoadRenderTexture(GAME_WIDTH, GAME_HEIGTH);

    BeginTextureMode(basic1);
    DrawRectangle(10,10,30,40,RED);
    EndTextureMode();

    BeginTextureMode(basic2);
    DrawRectangle(15,15,30,40,BLUE);
    EndTextureMode();

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
        // if(Key_Press(KEY_J)){
        //     speed_mod--;
        //     std::cout << "Decreased speed to " << pow(2, speed_mod) << std::endl;
        //     em.set_time_scale(pow(2, speed_mod));
        // }
        // if(Key_Press(KEY_L)){
        //     speed_mod++;
        //     std::cout << "Increased speed to " << pow(2, speed_mod) << std::endl;
        //     em.set_time_scale(pow(2, speed_mod));
        // }

        if(!paused){
            em.update();
        }

        BeginDrawing();
        ClearBackground(GRAY);
        for(auto& [layer_name, layer] : graphic_layers){
            DrawTexturePro(
                layer.texture,
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
        }
        new_frame();
        EndDrawing();
    }
    return 0;
}