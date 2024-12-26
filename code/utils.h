#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <vector>
#include <set>
#include <unordered_map>
#include <raylib.h>
#include <cmath>
#include <iostream>
#include <functional>

template<typename T>
bool exists(const std::vector<T>& v, const T& value) {
    if(v.size()==0){return false;}
    for(int i = 0; i<v.size(); i++){
        if(v[i]==value){return true;}
    } 
    return false;
};

template<typename T>
bool exists(const std::set<T>& v, const T& value) {
    if(v.size()==0){return false;}
    for(auto iter = v.begin(); iter != v.end(); ++iter){
        if(*iter == value){return true;}
    }
    return false;
};

template <typename K, typename V, typename... Maps>
std::unordered_map<K, V>union_maps(const std::unordered_map<K, V>& first, const Maps&... rest) {
    std::unordered_map<K, V> result = first; 
    (result.insert(rest.begin(), rest.end()), ...);
    return result;
};

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

//Vector math:
Vector2 operator+(const Vector2& lhs, const Vector2& rhs);

// Overload - operator
Vector2 operator-(const Vector2& lhs, const Vector2& rhs);

// Optional: Overload << operator for easy printing
std::ostream& operator<<(std::ostream& os, const Vector2& vec);

Vector2& operator*=(Vector2& vec, float scalar);

Vector2& operator/=(Vector2& vec, float scalar);

float magnitude(const Vector2& vec);

void rotate_vec(Vector2& vec, float angle);

void draw_vector(int x, int y, Vector2 vec, Color color);

void draw_arrow(int x, int y, int to_x, int to_y, Color color);

#endif