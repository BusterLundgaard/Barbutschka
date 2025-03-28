#ifndef EVENTS_H
#define EVENTS_H

#include <iostream>
#include <variant>
#include <optional>

enum EVENT{
    PLAYER_DEAD, 
    PLAYER_GROUNDED,
    PLAYER_UNGROUNDED,
    PLAYER_LEFT_SLOPE,
    PLAYER_ENTERED_SLOPE,
    NEW_SCREEN,
    OSCILLATOR_CHANGE
};

struct NO_DATA {};

struct PLAYER_GROUNDED_data{
    std::optional<int16_t> oscillator;
    std::optional<int16_t> slope;
};

struct PLAYER_ENTERED_SLOPE_data{
    int16_t slope;
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

typedef std::variant<PLAYER_DEAD_data, NEW_SCREEN_data, OSCILLATOR_CHANGE_data, PLAYER_GROUNDED_data, PLAYER_ENTERED_SLOPE_data, NO_DATA> Event_data;

#endif