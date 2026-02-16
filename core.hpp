#ifndef __MORLOC__CORE_HPP__
#define __MORLOC__CORE_HPP__

#include <vector>
#include <algorithm>
#include <functional>
#include <utility>
#include <map>
#include <stdexcept>
#include <sstream>
#include <cmath>

template <class A>
A morloc_id(const A& x){
  return x;
}


// Reverse a vector without mutating the input
template<typename T>
std::vector<T> morloc_reverse(const std::vector<T>& xs) {
    std::vector<T> result(xs);
    std::reverse(result.begin(), result.end());
    return result;
}


// Sort a vector without mutating the input
template<typename T>
std::vector<T> morloc_sort(const std::vector<T>& xs) {
    std::vector<T> result(xs);
    std::sort(result.begin(), result.end());
    return result;
}


// unzip :: [(a, b)] -> ([a], [b])
template <class A, class B>
std::tuple<std::vector<A>,std::vector<B>> morloc_unzip(const std::vector<std::tuple<A,B>> &xs){
    std::vector<A> a;
    std::vector<B> b;
    for(std::size_t i = 0; i < xs.size(); i++){
       a.push_back(std::get<0>(xs[i]));
       b.push_back(std::get<1>(xs[i]));
    }
    return std::make_tuple(a,b);
}


// keys :: Map a b -> [a]
template <typename Key, typename Value>
std::vector<Key> morloc_keys(const std::map<Key, Value>& map) {
    std::vector<Key> result;
    for (const auto& pair : map) {
        result.push_back(pair.first);
    }
    return result;
}

// vals :: Map a b -> [b]
template <typename Key, typename Value>
std::vector<Value> morloc_vals(const std::map<Key, Value>& map) {
    std::vector<Value> result;
    for (const auto& pair : map) {
        result.push_back(pair.second);
    }
    return result;
}

// fromList :: [(a, b)] -> Map a b
template <typename Key, typename Value>
std::map<Key, Value> morloc_from_list(const std::vector<std::tuple<Key, Value>>& tuples) {
    std::map<Key, Value> result;
    for (const auto& t : tuples) {
        result[std::get<0>(t)] = std::get<1>(t);
    }
    return result;
}

// mapKey :: (a -> a') -> Map a b -> Map a' b
template <typename Key, typename Value, typename NewKey>
std::map<NewKey, Value> morloc_map_key(std::function<NewKey(const Key&)> transform, const std::map<Key, Value>& map) {
    std::map<NewKey, Value> result;
    for (const auto& pair : map) {
        result[transform(pair.first)] = pair.second;
    }
    return result;
}

// mapVal :: (b -> b') -> Map a b -> Map a b'
template <typename Key, typename Value, typename NewValue>
std::map<Key, NewValue> morloc_map_val(std::function<NewValue(const Value&)> transform, const std::map<Key, Value>& map) {
    std::map<Key, NewValue> result;
    for (const auto& pair : map) {
        result[pair.first] = transform(pair.second);
    }
    return result;
}

template <class A, class INDEX>
A morloc_at(INDEX i, const std::vector<A>& xs){
    return xs[i];
}

template <class A, class INDEX>
std::vector<A> morloc_slice(INDEX i, INDEX j, std::vector<A> vec) {
    INDEX size = static_cast<INDEX>(vec.size());

    if (i < 0) i += size;
    if (j < 0) j += size;

    i = std::max(static_cast<INDEX>(0), std::min(i, size));
    j = std::max(static_cast<INDEX>(0), std::min(j, size));

    if (i > j) {
        return std::vector<A>();
    }

    return std::vector<A>(vec.begin() + i, vec.begin() + j);
}


template <class A>
A morloc_add(A x, A y){
    return x + y;
}

template <class A>
std::vector<A> morloc_list_add(const std::vector<A>& xs, const std::vector<A>& ys){
    std::vector<A> zs;
    zs.reserve(xs.size() + ys.size());
    zs.insert(zs.end(), xs.begin(), xs.end());
    zs.insert(zs.end(), ys.begin(), ys.end());
    return zs;
}

template <class A>
A morloc_sub(A x, A y){
    return x - y;
}

template <class A>
A morloc_mul(A x, A y){
    return x * y;
}

template <class A>
A morloc_div(A x, A y){
    return x / y;
}

template <class A>
A morloc_inv(A x){
    return 1 / x;
}

template <class A>
A morloc_neg(A x){
    return (-1) * x;
}

template <class A>
A morloc_mod(A x, A y){
    return x % y;
}

template <class A>
A morloc_abs(A x){
    return x < 0 ? -x : x;
}

template <class A>
A morloc_pow(A x, A y){
    return std::pow(x, y);
}

template <class A>
A morloc_ln(A x){
    return std::log(x);
}

template <class A>
std::string morloc_show(A x){
    std::ostringstream s;
    s << x;
    return(s.str());
}


template <class A, class B, class F>
std::vector<B> morloc_map(F f, const std::vector<A>& xs) {
    static_assert(std::is_invocable_r_v<B, F, A>,
                  "Function f must be callable with type A and return type B");
    std::vector<B> ys;
    ys.reserve(xs.size());
    for(const auto& x : xs) {
        ys.push_back(f(x));
    }
    return ys;
}

// Overload that deduces return type automatically
template <class A, class F>
auto morloc_map(F f, const std::vector<A>& xs) -> std::vector<std::invoke_result_t<F, A>> {
    return morloc_map<A, std::invoke_result_t<F, A>, F>(f, xs);
}


template <class A, class B, class C, class F>
std::vector<C> morloc_zipWith(
        F f,
        const std::vector<A>& xs,
        const std::vector<B>& ys
    )
{
    static_assert(std::is_invocable_r_v<C, F, A, B>,
                  "Function f must be callable with type A and return type B");
    std::size_t N = std::min(xs.size(), ys.size());
    std::vector<C> zs(N);
    for(std::size_t i = 0; i < N; i++){
        zs[i] = f(xs[i], ys[i]);
    }
    return zs;
}


template <class B, class A, class F>
B morloc_fold(F&& f, B y, const std::vector<A>& xs) {
    for(const auto& x : xs) {
        y = f(y, x);
    }
    return y;
}


// ifelse :: Bool -> a -> a -> a
template <class A>
A morloc_ifelse(bool cond, A x, A y){
    if (cond) {
        return x;
    } else {
        return y;
    }
}

// branch :: (a -> Bool) -> (a -> b) -> (a -> b) -> a -> b
template <class A, class B, class Cond, class F1, class F2>
B morloc_branch(Cond&& cond, F1&& f1, F2&& f2, const A& x) {
    if (cond(x)) {
        return f1(x);
    } else {
        return f2(x);
    }
}

// filter :: (a -> Bool) -> [a] -> [a]
template <class A>
std::vector<A> morloc_filter(
    std::function<bool(A)> f,
    const std::vector<A>& xs
){
    std::vector<A> ys;
    for(std::size_t i = 0; i < xs.size(); i++){
        if (f(xs[i])){
            ys.push_back(xs[i]);
        }
    }
    return ys;
}

// sortBy :: (a -> a -> Bool) -> [a] -> [a]
template <class A>
std::vector<A> morloc_sortBy(std::function<bool(A, A)> cmp, const std::vector<A>& xs) {
    std::vector<A> result(xs);
    std::sort(result.begin(), result.end(), cmp);
    return result;
}

// replicate :: Int -> a -> [a]
template <class A>
std::vector<A> morloc_replicate(int n, const A& x) {
    return std::vector<A>(n, x);
}

// takeWhile :: (a -> Bool) -> [a] -> [a]
template <class A>
std::vector<A> morloc_takeWhile(std::function<bool(A)> f, const std::vector<A>& xs) {
    std::vector<A> result;
    for (const auto& x : xs) {
        if (!f(x)) break;
        result.push_back(x);
    }
    return result;
}

// dropWhile :: (a -> Bool) -> [a] -> [a]
template <class A>
std::vector<A> morloc_dropWhile(std::function<bool(A)> f, const std::vector<A>& xs) {
    std::vector<A> result;
    bool dropping = true;
    for (const auto& x : xs) {
        if (dropping && f(x)) continue;
        dropping = false;
        result.push_back(x);
    }
    return result;
}

// partition :: (a -> Bool) -> [a] -> ([a], [a])
template <class A>
std::tuple<std::vector<A>, std::vector<A>> morloc_partition(std::function<bool(A)> f, const std::vector<A>& xs) {
    std::vector<A> yes, no;
    for (const auto& x : xs) {
        if (f(x)) yes.push_back(x);
        else no.push_back(x);
    }
    return std::make_tuple(yes, no);
}

// scanl :: (b -> a -> b) -> b -> [a] -> [b]
template <class B, class A>
std::vector<B> morloc_scanl(std::function<B(B, A)> f, B init, const std::vector<A>& xs) {
    std::vector<B> result;
    result.push_back(init);
    B acc = init;
    for (const auto& x : xs) {
        acc = f(acc, x);
        result.push_back(acc);
    }
    return result;
}

// intersperse :: a -> [a] -> [a]
template <class A>
std::vector<A> morloc_intersperse(const A& sep, const std::vector<A>& xs) {
    std::vector<A> result;
    for (std::size_t i = 0; i < xs.size(); i++) {
        if (i > 0) result.push_back(sep);
        result.push_back(xs[i]);
    }
    return result;
}

// enumerate :: [a] -> [(Int, a)]
template <class A>
std::vector<std::tuple<int, A>> morloc_enumerate(const std::vector<A>& xs) {
    std::vector<std::tuple<int, A>> result;
    for (int i = 0; i < static_cast<int>(xs.size()); i++) {
        result.push_back(std::make_tuple(i, xs[i]));
    }
    return result;
}

// lookup :: a -> Map a b -> b
template <class K, class V>
V morloc_lookup(const K& key, const std::map<K, V>& m) {
    auto it = m.find(key);
    if (it == m.end()) {
        throw std::runtime_error("Key not found in map");
    }
    return it->second;
}

// insert :: a -> b -> Map a b -> Map a b
template <class K, class V>
std::map<K, V> morloc_insert(const K& key, const V& val, const std::map<K, V>& m) {
    std::map<K, V> result(m);
    result[key] = val;
    return result;
}

// delete :: a -> Map a b -> Map a b
template <class K, class V>
std::map<K, V> morloc_delete(const K& key, const std::map<K, V>& m) {
    std::map<K, V> result(m);
    result.erase(key);
    return result;
}

// toList :: Map a b -> [(a, b)]
template <class K, class V>
std::vector<std::tuple<K, V>> morloc_to_list(const std::map<K, V>& m) {
    std::vector<std::tuple<K, V>> result;
    for (const auto& pair : m) {
        result.push_back(std::make_tuple(pair.first, pair.second));
    }
    return result;
}

// filterMap :: (a -> b -> Bool) -> Map a b -> Map a b
template <class K, class V>
std::map<K, V> morloc_filter_map(std::function<bool(K, V)> f, const std::map<K, V>& m) {
    std::map<K, V> result;
    for (const auto& pair : m) {
        if (f(pair.first, pair.second)) {
            result[pair.first] = pair.second;
        }
    }
    return result;
}


template <class A>
bool morloc_le(A x, A y){
   return x <= y;
}

template <class A>
bool morloc_eq(A x, A y){
   return x == y;
}


bool morloc_not(bool x){
    return !x;
}

bool morloc_and(bool x, bool y){
    return x && y;
}

bool morloc_or(bool x, bool y){
    return x || y;
}

#endif
