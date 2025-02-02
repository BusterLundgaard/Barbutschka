#include "graphic_layers.h"
#include "defs.h"
#include "globals.h"

std::map<GRAPHIC_LAYER, RenderTexture2D> graphic_layers = {
    {GRAPHIC_LAYER::static_level, {}}, 
    {GRAPHIC_LAYER::dynamic, {}},
    {GRAPHIC_LAYER::none, {}}
};

std::map<GRAPHIC_LAYER, bool> graphic_layer_refreshes = {
    {GRAPHIC_LAYER::static_level, false},
    {GRAPHIC_LAYER::dynamic, true},
    {GRAPHIC_LAYER::none, false}
};

void initialize_graphic_layers() {
    for(auto& [layer, texture] : graphic_layers){
        texture = LoadRenderTexture(GAME_WIDTH, GAME_HEIGTH);
    }
    BeginTextureMode(graphic_layers.at(GRAPHIC_LAYER::static_level));
}

void set_active_graphic_layer(GRAPHIC_LAYER layer) {
    auto graphic_layer = &graphic_layers.at(layer);
    if(layer != GRAPHIC_LAYER::none && graphic_layer != ACTIVE_GRAPHIC_LAYER){
        EndTextureMode();
        ACTIVE_GRAPHIC_LAYER = graphic_layer;
        BeginTextureMode(*ACTIVE_GRAPHIC_LAYER);
    }
}