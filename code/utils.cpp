#include <stdlib.h>
#include <vector>
#include <set>
#include <map>

template<typename T>
bool exists(const std::vector<T>& v, const T& value) {
    if(v.size()==0){return false;}
    for(int i = 0; i<v.size(); i++){
        if(v[i]==value){return true;}
    } 
    return false;
}
template<typename T>
bool exists(const std::set<T>& v, const T& value) {
    if(v.size()==0){return false;}
    for(auto iter = v.begin(); iter != v.end(); ++iter){
        if(*iter == value){return true;}
    }
    return false;
}

template <typename K, typename V, typename... Maps>
std::unordered_map<K, V>union_maps(const std::unordered_map<K, V>& first, const Maps&... rest) {
    std::unordered_map<K, V> result = first; 
    (result.insert(rest.begin(), rest.end()), ...);
    return result;
}