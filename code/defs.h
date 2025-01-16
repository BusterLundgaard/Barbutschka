#ifndef DEFS_H
#define DEFS_H

#include <iostream>
#include <typeindex>
#include <vector>
#include <unordered_map>

typedef std::type_index Typ;
typedef int16_t Id; 
template <typename T>
using V = std::vector<T>;
template <typename T1, typename T2>
using Map = std::unordered_map<T1, T2>;

const int GAME_WIDTH = 256;
const int GAME_HEIGTH = 176;
const int BLOCKS_X = 16;
const int BLOCKS_Y = 11;
const int BLOCK_SIZE = 16;

#define init_comp(compname) int compname::init = compname::initialize(); static Typ _ ## compname = typeid(compname);
#define init_meta static int init; static int initialize() 
#define default_meta(compname) metadata(typeid(compname), #compname, sizeof(compname)); 
#define manual_heap_meta(compname) \
    meta_set_heap_management( \
        typeid(compname), \
        [](void* src, void* dst) { \
            new (dst) compname(static_cast<compname*>(src)); \
        }, \
        [](void* src) { \
            compname* el = static_cast<compname*>(src); \
            el->~compname(); \
        } \
    )
#define manage_system_data(systemname) \ 
[](){\
    return static_cast<System_data*>(new systemname);\
},\
[](System_data* data_pointer){\
    delete static_cast<systemname*>(data_pointer);\
},

#endif