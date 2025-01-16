#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <set> 
#include <functional>
#include <raylib.h>
#include <math.h>
#include "defs.h"


template <typename T>
V<T*> convert_all(V<void*> els){
    V<T*> comp_typ_pointers(els.size());
    for(int i = 0; i < els.size(); i++){
        comp_typ_pointers[i] = static_cast<T*>(els[i]);
    }
    return comp_typ_pointers;
}

template <typename T>
bool exists(std::set<T> s, T el){
    return s.find(el) == s.end();
}

template <typename T>
bool exists(V<T> v, T el){
    return std::find(v.begin(), v.end(), el) != v.end();
}

template <typename T>
int count(V<T> v, T el){
    return std::count(v.begin(), v.end(), el);
}

template<typename T>
std::vector<T> filter(const std::vector<T>& v, std::function<bool(T)> f){
    std::vector<T> filtered;
    for(int i = 0; i < v.size(); i++){
        if(f(v[i])){
            filtered.push_back(v[i]);
        }
    }
    return filtered;
}

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

#endif