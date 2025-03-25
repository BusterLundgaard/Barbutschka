# Welcome to "Untitled Barbutschka Game"
Welcome, potential-future-employer-who-i-assume-is-the-only-person-potentially-reading-this-right now, welcome to my simple C++ ECS (Entity Component System) game engine. 

# How to compile and run (Tested on Arch Linux):
To comile:
`cd build && cmake .. && make`
To run:
`./main`

# Controls
This is only a short demo with one single screen, since the game still isn't very far along yet. You can use the arrow keys `LEFTARROW`, `RIGHTARROW` to move, `DOWNARROW` to duck (does nothing yet) and `Z` to jump. The longer you hold `Z`, the further your charachter will jump. 

Grandma sprite and animations are all original work. 

# Basic structure of the engine:
The actual game code is made in the `code/systems` folder, and in the `components.h` file. You write components as classes in `components.h`, and you write systems acting on those components in `code/systems`. 

The simplest example is probably `sys_draw_sprite` in `systems/drawing.cpp`:

```C++
Entity_individual_system sys_draw_sprite{
    "draw_sprite", //Name of the system (for debugging reasons)
    {__Sprite, __Transform}, {}, //Which components the system acts on
    GRAPHIC_LAYER::dynamic, //Which graphical layer the system acts on
    [](Id id){ //Lambda describing the actual system code 
        auto spr = em.get_from_entity<_Sprite>(id);
        auto t = em.get_from_entity<_Transform>(id);
        DrawTextureRec(
            spr->tex, 
            Rectangle{
                float((spr->tex).width)*spr->flipped, 0.0, (1 - 2*spr->flipped)*float((spr->tex).width), float((spr->tex).height)
            }, 
            Vector2{t->x, GAME_HEIGTH - t->y - spr->tex.height},
            WHITE
        );
    }
};
```

This system acts on a `Sprite` as defined in `components.h`:

```C++
class _Sprite : public Component {
    public:
    Texture2D tex;
    bool flipped;

    _Sprite(std::string path) : tex(LoadTexture(path.c_str())) {};
    _Sprite(std::string path, bool flipped) : tex(LoadTexture(path.c_str())), flipped(flipped) {};

    static int initialize() {
        default_meta(_Sprite, "Sprt");
        return 0;
    }
};
static Typ __Sprite = typeid(_Sprite);
```