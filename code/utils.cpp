#include <raylib.h>
#include <cmath>
#include <iostream>
#include "utils.h"

Vector2 operator+(const Vector2& lhs, const Vector2& rhs) {
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

// Overload - operator
Vector2 operator-(const Vector2& lhs, const Vector2& rhs) {
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

// Optional: Overload << operator for easy printing
std::ostream& operator<<(std::ostream& os, const Vector2& vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

Vector2& operator*=(Vector2& vec, float scalar) {
    vec.x *= scalar;
    vec.y *= scalar;
    return vec; // Return the modified vector
}

Vector2& operator/=(Vector2& vec, float scalar) {
    vec.x /= scalar;
    vec.y /= scalar;
    return vec; // Return the modified vector
}

float magnitude(const Vector2& vec){
    return sqrt(vec.x*vec.x + vec.y*vec.y);
}

void rotate_vec(Vector2& vec, float angle){
    Vector2 tmp = {vec.x, vec.y};
    vec.x = tmp.x * std::cos(angle) - tmp.y * std::sin(angle);
    vec.y = tmp.x * std::sin(angle) + tmp.y * std::cos(angle);
}

void draw_vector(int x, int y, Vector2 vec, Color color){
    DrawLine(x, y, x+vec.x, y+vec.y, color);
}

void draw_arrow(int x, int y, int to_x, int to_y, Color color){
    //draw base
    Vector2 vec = Vector2{static_cast<float>(to_x-x), static_cast<float>(to_y-y)};
    draw_vector(x, y, vec, color);
    
    //draw tail
    rotate_vec(vec, 0.52f);
    vec *= -6.0f/magnitude(vec);
    draw_vector(to_x, to_y, vec, color);
    rotate_vec(vec, -0.52f*2);
    draw_vector(to_x, to_y, vec, color);
}