#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>
#include <typeindex>
#include <set>
#include <fstream>
#include <sstream>
#include <limits>

class Component {
    public:
    Component() {}
    virtual ~Component() = default;
};

class A : public Component {
    int16_t a;
    public:
    A() : Component(), a(30) {}
};

class B : public Component {
    int64_t b;
    public:
    B() : Component(), b(45) {}
};

std::vector<Component*> comps;

void delete_all() {
    for(auto comp : comps){
        delete comp;
    }
}

int main() {
    comps.push_back(new A);
    comps.push_back(new B);
    delete_all();
    return 0;
}
