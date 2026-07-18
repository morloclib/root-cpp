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


// One template covers every IndexLike instance. U64 above 2^63-1 wraps to
// a negative int64_t; documented known issue.
//
// Returns std::optional<int64_t>: an absent input (std::nullopt) passes
// through so slicer bounds can be left blank (the desugar emits
// `(Null :: ?I64)` for an empty position, and a user expression
// evaluating to nullopt composes the same way). Non-empty values cast
// to int64_t and wrap in an Optional.
template <class T>
std::optional<int64_t> morloc_to_index(T x) {
    return std::optional<int64_t>{static_cast<int64_t>(x)};
}

template <class T>
std::optional<int64_t> morloc_to_index(std::optional<T> x) {
    return x ? std::optional<int64_t>{static_cast<int64_t>(*x)} : std::nullopt;
}

// Negative indices wrap from the end (Python semantics) so .[-1] returns the
// last element, .[-2] the second-to-last, and so on.
//
// __access_index__ takes ?I64 to match __to_index__'s return shape,
// but a nullopt index has no semantic meaning at runtime; throw.
template <class A>
A morloc_at(std::optional<int64_t> oi, const std::vector<A>& xs){
    if (!oi) throw std::runtime_error("morloc_at: index is nullopt");
    int64_t i = *oi;
    if (i < 0) i += static_cast<int64_t>(xs.size());
    return xs[i];
}

// Bounds arrive from morloc as ?I64; raw int literals coerce implicitly
// through std::optional<int64_t>'s converting constructor.
template <class A>
std::vector<A> morloc_slice(
    std::optional<int64_t> start,
    std::optional<int64_t> stop,
    std::optional<int64_t> step,
    std::vector<A> vec) {
    int64_t n = static_cast<int64_t>(vec.size());
    int64_t k = step.value_or(1);
    if (k == 0) {
        throw std::invalid_argument("slice step cannot be zero");
    }
    int64_t i, j;
    if (k > 0) {
        i = start.value_or(0);
        j = stop.value_or(n);
    } else {
        i = start.value_or(n - 1);
        j = stop.value_or(-1);
    }
    // Negative bounds wrap from the end (Python semantics), but only when the
    // bound was actually supplied -- a default of -1 for reverse step means
    // "one before index 0" and must not wrap.
    if (i < 0 && start.has_value()) i += n;
    if (j < 0 && stop.has_value()) j += n;
    if (k > 0) {
        i = std::max(static_cast<int64_t>(0), std::min(i, n));
        j = std::max(static_cast<int64_t>(0), std::min(j, n));
    } else {
        i = std::max(static_cast<int64_t>(-1), std::min(i, n - 1));
        j = std::max(static_cast<int64_t>(-1), std::min(j, n - 1));
    }
    std::vector<A> result;
    if (k > 0) {
        for (int64_t p = i; p < j; p += k) {
            result.push_back(vec[p]);
        }
    } else {
        for (int64_t p = i; p > j; p += k) {
            result.push_back(vec[p]);
        }
    }
    return result;
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
double morloc_to_real(A x){
    return static_cast<double>(x);
}

// -------------------- Numeric conversions --------------------
//
// Per-target thin wrappers around a master template. Morloc codegen calls
// each wrapper with an argument-deducible template, so the source type is
// picked up from the argument and the return type is fixed. This avoids
// needing return-type template arguments in the emitted call sites.

template<class To, class From>
inline To morloc_into_gen(From x){
    return static_cast<To>(x);
}

template<class F> inline uint16_t morloc_into_u16(F x){ return morloc_into_gen<uint16_t>(x); }
template<class F> inline uint32_t morloc_into_u32(F x){ return morloc_into_gen<uint32_t>(x); }
template<class F> inline uint64_t morloc_into_u64(F x){ return morloc_into_gen<uint64_t>(x); }
template<class F> inline int16_t  morloc_into_i16(F x){ return morloc_into_gen<int16_t>(x); }
template<class F> inline int32_t  morloc_into_i32(F x){ return morloc_into_gen<int32_t>(x); }
template<class F> inline int64_t  morloc_into_i64(F x){ return morloc_into_gen<int64_t>(x); }
template<class F> inline int      morloc_into_int(F x){ return morloc_into_gen<int>(x); }
template<class F> inline float    morloc_into_f32(F x){ return morloc_into_gen<float>(x); }
template<class F> inline double   morloc_into_f64(F x){ return morloc_into_gen<double>(x); }

// PartialInto: bounds-checked conversion. Fails by throwing on out-of-range,
// on non-integer floats, and on non-finite floats. Failures raise a
// std::runtime_error so the <Err> effect propagates to the nearest @catch.
// The float bounds are computed with ldexp so int64/uint64 boundaries are
// handled exactly (their max value cannot round-trip through double).

template<class To, class From>
inline To morloc_try_into_impl(From x){
    if constexpr (std::is_floating_point_v<From>){
        if (!std::isfinite(x)) throw std::runtime_error("cannot convert non-finite float to integer");
        if (std::trunc(x) != x) throw std::runtime_error("cannot convert non-integer float to integer");
        const int width = std::numeric_limits<To>::digits;
        double max_excl = std::ldexp(1.0, width);
        double min_val = std::is_signed_v<To> ? -std::ldexp(1.0, width) : 0.0;
        if (x < min_val || x >= max_excl) throw std::runtime_error("value out of range for target integer type");
        return static_cast<To>(x);
    } else {
        if constexpr (std::is_signed_v<From> && std::is_unsigned_v<To>){
            if (x < 0) throw std::runtime_error("cannot convert negative value to unsigned type");
            using U = std::make_unsigned_t<From>;
            if (static_cast<U>(x) > std::numeric_limits<To>::max()) throw std::runtime_error("value out of range for target integer type");
        } else if constexpr (std::is_unsigned_v<From> && std::is_signed_v<To>){
            if (x > static_cast<From>(std::numeric_limits<To>::max())) throw std::runtime_error("value out of range for target integer type");
        } else {
            if (x < std::numeric_limits<To>::min()) throw std::runtime_error("value out of range for target integer type");
            if (x > std::numeric_limits<To>::max()) throw std::runtime_error("value out of range for target integer type");
        }
        return static_cast<To>(x);
    }
}

template<class F> inline uint8_t  morloc_try_u8(F x) { return morloc_try_into_impl<uint8_t>(x); }
template<class F> inline uint16_t morloc_try_u16(F x){ return morloc_try_into_impl<uint16_t>(x); }
template<class F> inline uint32_t morloc_try_u32(F x){ return morloc_try_into_impl<uint32_t>(x); }
template<class F> inline uint64_t morloc_try_u64(F x){ return morloc_try_into_impl<uint64_t>(x); }
template<class F> inline int8_t   morloc_try_i8(F x) { return morloc_try_into_impl<int8_t>(x); }
template<class F> inline int16_t  morloc_try_i16(F x){ return morloc_try_into_impl<int16_t>(x); }
template<class F> inline int32_t  morloc_try_i32(F x){ return morloc_try_into_impl<int32_t>(x); }
template<class F> inline int64_t  morloc_try_i64(F x){ return morloc_try_into_impl<int64_t>(x); }
template<class F> inline int      morloc_try_int(F x){ return morloc_try_into_impl<int>(x); }

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

template <class A>
auto morloc_size(const A& x){
  return x.size();
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

template <class A>
A morloc_at(std::optional<int64_t> oi, const std::deque<A>& xs){
    if (!oi) throw std::runtime_error("morloc_at: deque index is nullopt");
    return xs[*oi];
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

// Packable List <-> Deque
// pack :: List a -> Deque a  (vector -> deque)
template <class A>
std::deque<A> morloc_packDeque(const std::vector<A>& xs) {
    return std::deque<A>(xs.begin(), xs.end());
}

// unpack :: Deque a -> List a  (deque -> vector)
template <class A>
std::vector<A> morloc_unpackDeque(const std::deque<A>& xs) {
    return std::vector<A>(xs.begin(), xs.end());
}

#endif
