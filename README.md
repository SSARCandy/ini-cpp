# C++ INI Parser

Yet another `.ini` parser for modern c++ (made for cpp17), inspired by [inih](https://github.com/benhoyt/inih)


## Example

```cpp
#include "ini/INIReader.h"
using namespace inireader;

int main() {
    INIReader r{"./test/fixtures/config.ini"};

    const std::string v = r.Get<std::string>("section1", "any"); // "5"
    const auto& v1 = r.Get<int>("section1", "any"); // 5
    const auto& v2 = r.Get<double>("section1", "any"); // 5.0

    const std::vector<float> v3{
        r.GetVector<float>("section2", "any_vec")
    }; // 1, 2, 3
    const std::vector<std::string> v4{
        r.GetVector<std::string>("section2", "any_vec")
    }; // "1", "2", "3"

    return 0;
}
```

The `config.ini`'s content is something looks like:

```
[section1]
any=5

[section2]
any_vec = 1 2 3
```

## Install

Simply copy the header file `ini/INIReader.h` to your project, then done. 
