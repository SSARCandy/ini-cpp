#include <iostream>
#include <string>
#include <typeinfo>
#include <boost/core/demangle.hpp>

#include "ini/INIReader.h"
using namespace inih;

namespace bc = boost::core;

int main() {
    INIReader r{"./test/fixtures/config.ini"};

    const auto& v1 = r.Get<std::string>("section1", "any");
    const auto& v2 = r.Get<int>("section1", "any");
    const auto& v3 = r.Get<double>("section1", "any");
    const auto& v4 = r.GetVector<float>("section2", "any_vec");
    const auto& v5{r.GetVector<std::string>("section2", "any_vec")};

    /** output
     * v1 = "1"             type: std::string
     * v2 = 1               type: int
     * v3 = 1.0             type: double
     * v4 = [1, 2, 3]       type: std::vector<float>
     * v5 = ["1", "2", "3"] type: std::vector<std::string>
     */

    std::cout << "v1 = " << v1 << ", which is type: " << bc::demangle(typeid(v1).name()) << std::endl;
    std::cout << "v2 = " << v2 << ", which is type: " << bc::demangle(typeid(v2).name()) << std::endl;
    std::cout << "v3 = " << v3 << ", which is type: " << bc::demangle(typeid(v3).name()) << std::endl;
    std::cout << "v4 = "; for (auto& v : v4) std::cout << v << " "; std::cout << ", which is type: " << bc::demangle(typeid(v4).name()) << std::endl;
    std::cout << "v5 = "; for (auto& v : v5) std::cout << v << " "; std::cout << ", which is type: " << bc::demangle(typeid(v5).name()) << std::endl;

    return 0;
}
