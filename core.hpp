#ifndef __MORLOC__CORE_HPP__
#define __MORLOC__CORE_HPP__

#include <vector>
#include <deque>
#include <algorithm>
#include <functional>
#include <utility>
#include <stdexcept>
#include <sstream>
#include <map>
#include <cmath>
#include <optional>
#include <set>
#include <type_traits>

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


// List (vector) concat
template <class A>
std::vector<A> morloc_list_add(const std::vector<A>& xs, const std::vector<A>& ys){
    std::vector<A> zs;
    zs.reserve(xs.size() + ys.size());
    zs.insert(zs.end(), xs.begin(), xs.end());
    zs.insert(zs.end(), ys.begin(), ys.end());
    return zs;
}

// Deque concat
template <class A>
std::deque<A> morloc_list_add(const std::deque<A>& xs, const std::deque<A>& ys){
    std::deque<A> zs(xs);
    zs.insert(zs.end(), ys.begin(), ys.end());
    return zs;
}

// Integer division for floats: divide then floor
// e.g. floor_div(-7.0, 2.0) == -4.0  (not -3.0)
template <class A>
A morloc_floor_div(A x, A y) {
    return std::floor(x / y);
}

// Modulus for floats, satisfying: x == y * floor_div(x, y) + mod(x, y)
// e.g. float_mod(-7.0, 2.0) == 1.0  (not -1.0 as fmod would give)
template <class A>
A morloc_float_mod(A x, A y) {
    return x - y * std::floor(x / y);
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


// --- map (Functor) ---

// Vector overloads
template <class A, class F>
auto morloc_map(F f, const std::vector<A>& xs) -> std::vector<std::invoke_result_t<F, A>> {
    using B = std::invoke_result_t<F, A>;
    std::vector<B> ys;
    ys.reserve(xs.size());
    for(const auto& x : xs) {
        ys.push_back(f(x));
    }
    return ys;
}

// Deque overload
template <class A, class F>
auto morloc_map(F f, const std::deque<A>& xs) -> std::deque<std::invoke_result_t<F, A>> {
    using B = std::invoke_result_t<F, A>;
    std::deque<B> ys;
    for(const auto& x : xs) {
        ys.push_back(f(x));
    }
    return ys;
}


template <class A, class B, class F>
auto morloc_zipWith(F f, const std::vector<A>& xs, const std::vector<B>& ys)
    -> std::vector<std::invoke_result_t<F, A, B>>
{
    using C = std::invoke_result_t<F, A, B>;
    std::size_t N = std::min(xs.size(), ys.size());
    std::vector<C> zs(N);
    for(std::size_t i = 0; i < N; i++){
        zs[i] = f(xs[i], ys[i]);
    }
    return zs;
}


// --- fold (Foldable) ---

// Vector overloads
template <class B, class A, class F>
B morloc_fold(F&& f, B y, const std::vector<A>& xs) {
    for(const auto& x : xs) {
        y = f(y, x);
    }
    return y;
}

template <class A, class F>
A morloc_fold1(F&& f, const std::vector<A>& xs) {
    A acc = xs[0];
    for(std::size_t i = 1; i < xs.size(); i++) {
        acc = f(acc, xs[i]);
    }
    return acc;
}

template <class A, class F>
std::optional<A> morloc_safeFold1(F&& f, const std::vector<A>& xs) {
    if(xs.empty()) {
        return std::nullopt;
    }
    A acc = xs[0];
    for(std::size_t i = 1; i < xs.size(); i++) {
        acc = f(acc, xs[i]);
    }
    return acc;
}

// Deque overloads
template <class B, class A, class F>
B morloc_fold(F&& f, B y, const std::deque<A>& xs) {
    for(const auto& x : xs) {
        y = f(y, x);
    }
    return y;
}

template <class A, class F>
A morloc_fold1(F&& f, const std::deque<A>& xs) {
    A acc = xs[0];
    for(std::size_t i = 1; i < xs.size(); i++) {
        acc = f(acc, xs[i]);
    }
    return acc;
}

template <class A, class F>
std::optional<A> morloc_safeFold1(F&& f, const std::deque<A>& xs) {
    if(xs.empty()) {
        return std::nullopt;
    }
    A acc = xs[0];
    for(std::size_t i = 1; i < xs.size(); i++) {
        acc = f(acc, xs[i]);
    }
    return acc;
}


// filter :: (a -> Bool) -> [a] -> [a]
template <class A, class F>
std::vector<A> morloc_filter(F f, const std::vector<A>& xs){
    std::vector<A> ys;
    for(std::size_t i = 0; i < xs.size(); i++){
        if (f(xs[i])){
            ys.push_back(xs[i]);
        }
    }
    return ys;
}

// sortBy :: (a -> a -> Bool) -> [a] -> [a]
template <class A, class F>
std::vector<A> morloc_sortBy(F cmp, const std::vector<A>& xs) {
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
template <class A, class F>
std::vector<A> morloc_takeWhile(F f, const std::vector<A>& xs) {
    std::vector<A> result;
    for (const auto& x : xs) {
        if (!f(x)) break;
        result.push_back(x);
    }
    return result;
}

// dropWhile :: (a -> Bool) -> [a] -> [a]
template <class A, class F>
std::vector<A> morloc_dropWhile(F f, const std::vector<A>& xs) {
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
template <class A, class F>
std::tuple<std::vector<A>, std::vector<A>> morloc_partition(F f, const std::vector<A>& xs) {
    std::vector<A> yes, no;
    for (const auto& x : xs) {
        if (f(x)) yes.push_back(x);
        else no.push_back(x);
    }
    return std::make_tuple(yes, no);
}

// scanl :: (b -> a -> b) -> b -> [a] -> [b]
template <class B, class A, class F>
std::vector<B> morloc_scanl(F f, B init, const std::vector<A>& xs) {
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

// --- Stack operations ---

// cons :: a -> [a] -> [a]  (vector)
template <class A>
std::vector<A> morloc_cons(const A& x, const std::vector<A>& xs) {
    std::vector<A> result;
    result.reserve(xs.size() + 1);
    result.push_back(x);
    result.insert(result.end(), xs.begin(), xs.end());
    return result;
}

// uncons :: [a] -> (a, [a])  (vector)
template <class A>
std::tuple<A, std::vector<A>> morloc_uncons(const std::vector<A>& xs) {
    return std::make_tuple(xs[0], std::vector<A>(xs.begin() + 1, xs.end()));
}

// cons :: a -> Deque a -> Deque a
template <class A>
std::deque<A> morloc_cons(const A& x, const std::deque<A>& xs) {
    std::deque<A> result(xs);
    result.push_front(x);
    return result;
}

// uncons :: Deque a -> (a, Deque a)
template <class A>
std::tuple<A, std::deque<A>> morloc_uncons(const std::deque<A>& xs) {
    std::deque<A> tail(xs.begin() + 1, xs.end());
    return std::make_tuple(xs.front(), tail);
}

// --- Queue operations ---

// snoc :: [a] -> a -> [a]  (vector)
template <class A>
std::vector<A> morloc_snoc(const std::vector<A>& xs, const A& x) {
    std::vector<A> result(xs);
    result.push_back(x);
    return result;
}

// unsnoc :: [a] -> ([a], a)  (vector)
template <class A>
std::tuple<std::vector<A>, A> morloc_unsnoc(const std::vector<A>& xs) {
    return std::make_tuple(std::vector<A>(xs.begin(), xs.end() - 1), xs.back());
}

// snoc :: Deque a -> a -> Deque a
template <class A>
std::deque<A> morloc_snoc(const std::deque<A>& xs, const A& x) {
    std::deque<A> result(xs);
    result.push_back(x);
    return result;
}

// unsnoc :: Deque a -> (Deque a, a)
template <class A>
std::tuple<std::deque<A>, A> morloc_unsnoc(const std::deque<A>& xs) {
    std::deque<A> init(xs.begin(), xs.end() - 1);
    return std::make_tuple(init, xs.back());
}

// --- Indexed for Deque ---

template <class A, class INDEX>
A morloc_at(INDEX i, const std::deque<A>& xs){
    return xs[i];
}

template <class A, class INDEX>
std::deque<A> morloc_slice(INDEX i, INDEX j, std::deque<A> dq) {
    INDEX size = static_cast<INDEX>(dq.size());

    if (i < 0) i += size;
    if (j < 0) j += size;

    i = std::max(static_cast<INDEX>(0), std::min(i, size));
    j = std::max(static_cast<INDEX>(0), std::min(j, size));

    if (i > j) {
        return std::deque<A>();
    }

    return std::deque<A>(dq.begin() + i, dq.begin() + j);
}

// --- Other list operations ---

// iterate :: Int -> (a -> a) -> a -> [a]
template <class A, class F>
std::vector<A> morloc_iterate(int n, F&& f, A x) {
    std::vector<A> result;
    result.reserve(n);
    for (int i = 0; i < n; i++) {
        result.push_back(x);
        x = f(x);
    }
    return result;
}

// groupBy :: (a -> a -> Bool) -> [a] -> [[a]]
template <class A, class F>
std::vector<std::vector<A>> morloc_groupBy(F eq, const std::vector<A>& xs) {
    std::vector<std::vector<A>> result;
    if (xs.empty()) return result;
    std::vector<A> group;
    group.push_back(xs[0]);
    for (std::size_t i = 1; i < xs.size(); i++) {
        if (eq(xs[i-1], xs[i])) {
            group.push_back(xs[i]);
        } else {
            result.push_back(group);
            group.clear();
            group.push_back(xs[i]);
        }
    }
    result.push_back(group);
    return result;
}

// find :: (a -> Bool) -> [a] -> ?a
template <class A, class F>
std::optional<A> morloc_find(F f, const std::vector<A>& xs) {
    for (const auto& x : xs) {
        if (f(x)) return x;
    }
    return std::nullopt;
}

// unique :: [a] -> [a]
template <class A>
std::vector<A> morloc_unique(const std::vector<A>& xs) {
    std::vector<A> result;
    std::set<A> seen;
    for (const auto& x : xs) {
        if (seen.find(x) == seen.end()) {
            seen.insert(x);
            result.push_back(x);
        }
    }
    return result;
}

// groupSort :: [(a, b)] -> [(a, [b])]
template <class A, class B>
std::vector<std::tuple<A, std::vector<B>>> morloc_groupSort(const std::vector<std::tuple<A, B>>& xs) {
    std::map<A, std::vector<B>> groups;
    for (const auto& t : xs) {
        groups[std::get<0>(t)].push_back(std::get<1>(t));
    }
    std::vector<std::tuple<A, std::vector<B>>> result;
    for (const auto& pair : groups) {
        result.push_back(std::make_tuple(pair.first, pair.second));
    }
    return result;
}

// range :: Int -> Int -> [Int]  (half-open interval [a, b))
std::vector<int> morloc_range(int a, int b) {
    std::vector<int> result;
    for (int i = a; i < b; i++) {
        result.push_back(i);
    }
    return result;
}

// rangeStep :: Int -> Int -> Int -> [Int]  (half-open interval [a, b))
std::vector<int> morloc_rangeStep(int a, int b, int step) {
    std::vector<int> result;
    for (int i = a; i < b; i += step) {
        result.push_back(i);
    }
    return result;
}

// read :: Str -> ?Int
std::optional<int> morloc_read_int(const std::string& s) {
    try {
        std::size_t pos;
        int val = std::stoi(s, &pos);
        if (pos == s.size()) return val;
        return std::nullopt;
    } catch (...) {
        return std::nullopt;
    }
}

// read :: Str -> ?Real
std::optional<double> morloc_read_real(const std::string& s) {
    try {
        std::size_t pos;
        double val = std::stod(s, &pos);
        if (pos == s.size()) return val;
        return std::nullopt;
    } catch (...) {
        return std::nullopt;
    }
}

// read :: Str -> ?Str
std::optional<std::string> morloc_read_str(const std::string& s) {
    return s;
}

// read :: Str -> ?Bool
std::optional<bool> morloc_read_bool(const std::string& s) {
    if (s == "true" || s == "True" || s == "TRUE") return true;
    if (s == "false" || s == "False" || s == "FALSE") return false;
    return std::nullopt;
}


template <class A>
bool morloc_le(A x, A y){
   return x <= y;
}

template <class A>
bool morloc_eq(A x, A y){
   return x == y;
}

// optional<A> vs nullopt
template <class A>
bool morloc_eq(const std::optional<A>& x, std::nullopt_t) {
   return !x.has_value();
}

template <class A>
bool morloc_eq(std::nullopt_t, const std::optional<A>& x) {
   return !x.has_value();
}

// nullopt vs nullopt
inline bool morloc_eq(std::nullopt_t, std::nullopt_t) {
   return true;
}

// non-optional vs nullopt (always false: a concrete value is never null)
template <class A, typename = std::enable_if_t<!std::is_same_v<A, std::nullopt_t>>>
bool morloc_eq(const A&, std::nullopt_t) {
   return false;
}

template <class A, typename = std::enable_if_t<!std::is_same_v<A, std::nullopt_t>>>
bool morloc_eq(std::nullopt_t, const A&) {
   return false;
}

// non-optional vs optional (from coercion: value compared to typed optional)
template <class A>
bool morloc_eq(const A& x, const std::optional<A>& y) {
   return y.has_value() && morloc_eq(x, *y);
}

template <class A>
bool morloc_eq(const std::optional<A>& x, const A& y) {
   return x.has_value() && morloc_eq(*x, y);
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

// --- Sequence conversions ---

// toDeque :: [a] -> Deque a
template <class A>
std::deque<A> morloc_toDeque(const std::vector<A>& xs) {
    return std::deque<A>(xs.begin(), xs.end());
}

// toVector :: [a] -> Vector a  (identity in C++, both are std::vector)
template <class A>
std::vector<A> morloc_toVector(const std::vector<A>& xs) {
    return xs;
}

// toArray :: [a] -> Array a  (identity in C++, both are std::vector)
template <class A>
std::vector<A> morloc_toArray(const std::vector<A>& xs) {
    return xs;
}

#endif
