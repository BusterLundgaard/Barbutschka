#include <raylib.h>
#include <memory>

#include "code/ecs_manager.h"
#include "code/systems.h"
#include "code/constants.h"

#define cast(var) static_cast<Component*>(&var)

std::vector<System*> systems = {
    &sys_velocity, 
    &sys_update_collider_global_pos, 
    &sys_find_collider_hits,
    &sys_draw_sprite, 
    &sys_DEBUG_draw_hit_collider,
    &sys_draw_world
    };

int main() {
    
    std::unordered_map<std::type_index, BaseComponentMap*> component_lists = {};
    std::unordered_map<std::type_index, std::string> type_index_to_string = {};
    
    set_component_lists(component_lists, type_index_to_string);
    ECS_manager em(component_lists, type_index_to_string, systems);

    // Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(GAME_WIDTH*4, GAME_HEIGTH*4, "Hello Raylib");
	SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGTH);

    {
        _Level lvl("../Tiled/test.csv", "../Tiled/basic.png");
        em.add_component(0, cast(lvl));
    }

    {
        _Transform t(30, 45);
        _HitCollider col(0, 0, 40, 5);

        em.add_component(1, cast(t));
        em.add_component(1, cast(col));

    }
    {
        _Transform t2(-49, 85);
        _HitCollider col2(0, 0, 43, 40, true);
         em.add_component(2, cast(t2));
        em.add_component(2, cast(col2));

        _Velocity v(0.3f, 0);
        em.add_component(2, cast(v));
    }
    {
        _Transform t(133, 55);
        em.add_component(3, cast(t));

        _HitCollider col(0, 0, 70, 30);
        em.add_component(3, cast(col));
    }
    {
        _Transform t(186, 69);
        em.add_component(4, cast(t));

        _HitCollider col(0, 0, 29, 34);
        em.add_component(4, cast(col));
    }
    {
        _Transform t(167, 91);
        em.add_component(5, cast(t));

        _HitCollider col(0, 0, 30, 27);
        em.add_component(5, cast(col));
    }
    {
        _Transform t(182, 96);
        em.add_component(6, cast(t));

        _HitCollider col(0, 0, 29, 27);
        em.add_component(6, cast(col));
    }

    while(!WindowShouldClose()){

        float scale = std::min(
            (float)GetScreenWidth()/GAME_WIDTH, 
            (float)GetScreenHeight()/GAME_HEIGTH
        );

        BeginTextureMode(target);
        ClearBackground(BLACK);
        em.update();

        EndTextureMode();
        
        BeginDrawing();
        ClearBackground(BLACK);
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
        EndDrawing();
    }
    return 0;
}

