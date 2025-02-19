#ifndef GLOBALS_H
#define GLOBALS_H

#include <defs.h>
#include <raylib.h>
#include <ecs_m.h>

extern Ecs_m em;
extern int GRID[BLOCKS_Y][BLOCKS_X];
extern int ROOM_X;
extern int ROOM_Y;
extern int PLAYER_ID;
extern RenderTexture2D* ACTIVE_GRAPHIC_LAYER;

#endif