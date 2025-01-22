#ifndef EVENTS_H
#define EVENTS_H

#include <iostream>
#include <variant>

enum Event{
    PLAYER_DEAD, 
    NEW_SCREEN,
    OSCILLATOR_CHANGE
};

struct PLAYER_DEAD_data{
    std::string name;
    int height;
};

struct NEW_SCREEN_data{
    int screen_x;
    int screen_y;
};

struct OSCILLATOR_CHANGE_data{
    bool direction;
};

typedef std::variant<PLAYER_DEAD_data, NEW_SCREEN_data, OSCILLATOR_CHANGE_data> Event_data;

#endif