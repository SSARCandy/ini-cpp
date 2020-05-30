#include <iostream>
#include <vector>
#include <string>
#include "ini/INIReader.h"
using namespace inireader;

int main() {
    INIReader r{"./test/fixtures/config.ini"};

    const std::string v = r.Get<std::string>("section1", "any"); 
    const auto& v1 = r.Get<int>("section1", "any");
    const auto& v2 = r.Get<double>("section1", "any");

    const std::vector<float> v3{r.GetVector<float>("section2", "any_vec")};
    const std::vector<std::string> v4{r.GetVector<std::string>("section2", "any_vec")};

    return 0;
}