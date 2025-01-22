#include "input.h"
#include <raylib.h>
#include <iostream>

// Global variable definition
std::unordered_map<int, bool> P_Is_Key_Pressed = {};

void new_frame() {
    for (auto iter = P_Is_Key_Pressed.begin(); iter != P_Is_Key_Pressed.end(); ++iter) {
        P_Is_Key_Pressed.insert_or_assign(iter->first, IsKeyDown(iter->first));
    }
}

void register_key(int key) {
    P_Is_Key_Pressed.insert({key, false});
}

bool Key_Press(int key) {
    return IsKeyDown(key) && !P_Is_Key_Pressed.at(key);
}

bool Key_Up(int key) {
    return !IsKeyDown(key) && P_Is_Key_Pressed.at(key);
}