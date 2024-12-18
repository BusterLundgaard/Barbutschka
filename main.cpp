#include <raylib.h>
#include <memory>

#include "code/ecs_manager.h"
#include "code/systems.h"
#include "code/constants.h"

#define cast(var) static_cast<Component*>(&var)

std::vector<System*> systems = {&sys_velocity, &sys_draw_sprite};

int main() {
    
    std::unordered_map<std::type_index, BaseComponentMap*> component_lists = {};
    set_component_lists(component_lists);
    ECS_manager em(component_lists, systems);

    // Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(GAME_WIDTH*3, GAME_HEIGTH*3, "Hello Raylib");
	SetTargetFPS(60);

    RenderTexture2D target = LoadRenderTexture(GAME_WIDTH, GAME_HEIGTH);

    {
    _Sprite wabbit("../assets/wabbit_alpha.png");
    em.add_component(0, cast(wabbit));
    }
    {
    _Transform t(10, 30);
    em.add_component(0, cast(t));
    }
    {
    _Velocity v(2,1);
    em.add_component(0, cast(v));
    }

    {
    _Sprite wabbit("../assets/wabbit_alpha.png");
    em.add_component(1, cast(wabbit));
    }
    {
    _Transform t(100, 50);
    em.add_component(1, cast(t));
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

