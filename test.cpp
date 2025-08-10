#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_NO_POSIX_SIGNALS // Add this line
#include "doctest.hpp"
#include "core.hpp"
#include "data.hpp"
#include <vector>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <map>
#include <utility>

int test_run(){
    return 5;
}

template <class A>
A inc(A x){
    return x + 1;
}

std::string makeName(std::string s, size_t i){
    std::ostringstream output;
    output << s << "_" << i;
    return output.str();
}

std::string reverseInt(int x){
    std::ostringstream output;
    output << x;
    std::string s = output.str();
    std::reverse(s.begin(), s.end());
    return s;
}

TEST_CASE("Higher Order Functions"){
    std::vector<int> xs{12,23,134};
    std::vector<int> ys_exp{13,24,135};
    std::function<int(int)> incFun = static_cast<int(*)(int)>(&inc);
    std::vector<int> ys = morloc_map(incFun, xs);
    CHECK(ys == ys_exp);

    std::function<std::string(int)> reverseIntFun = static_cast<std::string(*)(int)>(&reverseInt);
    std::vector<std::string> ss = morloc_map(reverseIntFun, xs);
    std::vector<std::string> ss_exp = {"21", "32", "431"};
    CHECK(ss == ss_exp);

    std::function<int(int,int)> addFun = static_cast<int(*)(int,int)>(&morloc_add);
    std::vector<int> zs = morloc_zipWith(addFun, xs, xs);
    std::vector<int> zs_exp{24,46,268};
    CHECK(zs == zs_exp);

    double b = 0.0;
    std::vector<double> ws{12.1,23.2,134.3};
    std::function<double(double,double)> addFunDbl = static_cast<double(*)(double,double)>(&morloc_add);
    double summed = morloc_fold(addFunDbl, b, ws);
    CHECK(summed - 169.6 + 0.01 < 0.02);

    std::vector<std::string> bases{"asdf", "qwer", "wert"};

    std::function<std::string(std::string,size_t)> makeNameFun =
        static_cast<std::string(*)(std::string,size_t)>(&makeName);
    std::vector<std::string> names = morloc_enumerateWith(makeNameFun, bases);
    std::vector<std::string> names_exp = {"asdf_0", "qwer_1", "wert_2"};
    CHECK(names == names_exp);
}

TEST_CASE("Control"){
    int x = morloc_run(std::function<int()>(test_run));
    CHECK(x == 5);
}

TEST_CASE("Serialization"){
    std::vector<std::string> xs = {"frogs", "oranges", "pretzels"};    
    std::vector<int> ys = {1,5,11};
    std::tuple<std::vector<std::string>,std::vector<int>> p = std::make_tuple(xs, ys);
    CHECK(morloc_unpackMap(morloc_packMap(p)) == p);
}
