#ifndef __MORLOC_CPPBASE_DATA_HPP__
#define __MORLOC_CPPBASE_DATA_HPP__

#include <vector>
#include <map>
#include <utility>
#include <cassert>
#include "mlccpptypes/prelude.hpp"

template <class A, class B>
std::map<A,B> morloc_packMap(std::tuple<std::vector<A>,std::vector<B>> items){
    std::map<A,B> m;
    std::vector<A> a = std::get<0>(items);
    std::vector<B> b = std::get<1>(items);
    assert(a.size() == b.size());
    for(std::size_t i = 0; i < a.size(); i++){
        m[a[i]] = b[i];
    }
    return m;
}

template <class A, class B>
std::tuple<std::vector<A>,std::vector<B>> morloc_unpackMap(std::map<A,B> m){
    std::vector<A> a;
    std::vector<B> b;
    for (auto tuple : m) {
        a.push_back(std::get<0>(tuple));
        b.push_back(std::get<1>(tuple));
    }
    return std::make_tuple(a, b);
}

// packUnit Cpp :: pack => "mlc::Unit" -> "int"
mlc::Unit morloc_packUnit(int u){
    return mlc::Unit();
}

// unpackUnit Cpp :: unpack => "int" -> "mlc::Unit"
int morloc_unpackUnit(mlc::Unit x){
    return 0;
}

#endif
