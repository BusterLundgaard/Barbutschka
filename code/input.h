#ifndef INPUT_H
#define INPUT_H

#include <unordered_map>

// Global variable declaration
extern std::unordered_map<int, bool> P_Is_Key_Pressed;

// Function declarations
void new_frame();
void register_key(int key);
bool Key_Press(int key);
bool Key_Up(int key);

#endif // INPUT_H