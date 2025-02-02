#ifndef GRAPHIC_LAYERS_H
#define GRAPHIC_LAYERS_H

#include <raylib.h>
#include <map>

enum GRAPHIC_LAYER {
    none,
    static_level, 
    dynamic
};

void initialize_graphic_layers();
void set_active_graphic_layer(GRAPHIC_LAYER layer);

extern std::map<GRAPHIC_LAYER, bool> graphic_layer_refreshes;
extern std::map<GRAPHIC_LAYER, RenderTexture2D> graphic_layers;

#endif