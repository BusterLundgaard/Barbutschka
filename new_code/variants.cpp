#include <variant>
#include <iostream>
#include <typeindex>
#include <vector>
#include <unordered_map>

typedef int16_t Id;
typedef std::type_index Typ;
template <typename T>
using V = std::vector<T>;

static int fresh_id = 0;

class Component {
    public:
    Id comp_id;
    Typ typ;
    Component(Typ typ) : comp_id(fresh_id++), typ(typ) {}
    virtual ~Component() = default;
};

class A : public Component{
    public:
    int a;
    A(int a) : Component(typeid(A)), a(a) {}
};

class B : public Component{
    public:
    long int b;
    B(long int b) : Component(typeid(B)), b(b) {}
};

class C : public Component{
    public:
    std::string c;
    C(std::string c) : Component(typeid(C)), c(c) {}
};

typedef std::variant<V<A>,V<B>,V<C>> comp_var;


// So, how to do this (assuming we can erase at an index and swap without knowing type)

// ECS_manager holds a map from Typ to Variant<CompList>
// CompList holds a Variant<V<Comp1>, V<Comp2>, ect>

// ECS_manager has two templated functions: 
// void add<T>(T element, Id entity_id):
// 	std::get<V<T>>(compLists[typeid(T)].data).push_back(element);
// 	add_element(element.comp_id, entity_id) //some private non-templated function
// T& get<T>(Id comp_id):
// 	return std::get<V<T>>(compLists[typeid(T)].data)[comp_id_to_index[typeid(T)][comp_id]];

// Our delete(comp_id) method won't need templating:
// void delete(Id comp_id) {
// 	Typ typ = comp_id_to_typ[comp_id];
// 	CompList compList = compLists[typ];
// 	Id last_id = compList.get_last_id();
	
// 	compList.swap_with_last(comp_id_to_index[typ]);
// 	compList.remove_last();
	
// 	//then now do the swapping in all the maps inside ECS_manager   
// }

	
class MyClass {
public:
    using VecVariant = std::variant<std::vector<A>, std::vector<B>, std::vector<C>>;

private:
    VecVariant data;

public:
    // Constructor to initialize with a vector of A, B, or C
    template <typename T>
    MyClass(std::vector<T> vec) {
        data = std::move(vec); // Use move semantics to initialize the variant
    }

    // Method to get the comp_id of the last element in the vector
    int16_t get_last_comp_id() const {
        return std::visit([](const auto& vec) -> int16_t {
            if (vec.empty()) {
                throw std::out_of_range("Vector is empty");
            }
            return vec.back().comp_id; // Return the comp_id of the last element
        }, data); // Apply the function to the variant
    }

    // Method to print the current contents of the vector
    void print() const {
        std::visit([](const auto& vec) {
            for (const auto& elem : vec) {
                std::cout << "comp_id: " << elem.comp_id << " ";
            }
            std::cout << std::endl;
        }, data);
    }
};






class ECS_m {
    std::unordered_map<Typ, comp_var> comp_vecs;
    std::unordered_map<Id, Id> comp_id_to_entity_id;
    std::unordered_map<Typ, std::unordered_map<Id, int>> id_to_index_maps;       

    template <typename T>
    void remove_comp(Id comp_id){
        V<T> vec = std::get<V<T>>(comp_vecs[typeid(T)]);
        int index = id_to_index_maps[typeid(T)][comp_id];

        T last = vec[vec.size()-1];
        vec[vec.size()-1] = vec[index];
        vec[index]=last;
        vec.pop_back();

        id_to_index_maps.erase(comp_id);
        id_to_index_maps[last.comp_id]=index;
    }

    public:
    ECS_m() {
        comp_vecs = {
            {typeid(A), comp_var {V<A> {}}},
            {typeid(B), comp_var {V<B> {}}},
            {typeid(C), comp_var {V<C> {}}}
        };
    }

    template <typename T> 
    T get(Id comp_id) {
        return std::get<V<T>>(comp_vecs[typeid(T)])[id_to_index_maps[typeid(T)][comp_id]];         
    }  

    



};

int main() {
    std::unordered_map<Typ, comp_var> comp_vecs = {
        {typeid(A), comp_var {V<A>{A(3)}}},
        {typeid(B), comp_var {V<B>{B(2), B(4)}}},
        {typeid(C), comp_var {V<C>{C("Hello"), C("Damn")}}}
    };
    std::cout << std::get<V<B>>(comp_vecs.at(typeid(B)))[1].b;      
    

    // std::variant<int, std::string> var = 42;
    // std::cout << std::get<int>(var) << std::endl;
    // var = "hello";
    // std::cout << std::get<int>(var) << std::endl;


    return 0;
}