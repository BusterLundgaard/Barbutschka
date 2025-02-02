#ifndef FLAGS_H
#define FLAGS_H

#include <cstdint>

typedef int64_t flag;

const flag F_No_flag = 0;
const flag F_Adjustable = 1 << 0;
const flag F_Non_adjustable = 1 << 1;
const flag F_Solid = 1 << 2;
const flag F_Debug_collision = 1 << 3;
const flag AMOUNT_OF_FLAGS = 4;

#endif