# C++ INI Parser

[![C/C++ CI](https://github.com/SSARCandy/ini-cpp/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/SSARCandy/ini-cpp/actions/workflows/c-cpp.yml)
[![codecov](https://codecov.io/gh/SSARCandy/ini-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/SSARCandy/ini-cpp)
  
Yet another `.ini` parser for modern c++ (made for cpp17), inspired and extend from [inih](https://github.com/benhoyt/inih).


## Example

The `config.ini`'s content is something looks like:

```
[section1]
any=5

[section2]
any_vec = 1 2 3
```

```cpp
#include "ini/ini.h"

int main() {
    inih::INIReader r{"./test/fixtures/config.ini"};

    // Get and parse the ini value
    const auto& v1 = r.Get<std::string>("section1", "any"); // "5"
    const auto& v2 = r.Get<int>("section1", "any"); // 5
    const auto& v3 = r.Get<double>("section1", "any"); // 5.0
    const auto& v4 = r.GetVector<float>("section2", "any_vec"); // [1.0, 2.0, 3.0]
    const auto& v5 = r.GetVector<std::string>("section2", "any_vec"); // ["1", "2", "3"]

    // And also support writing to new ini file.
    r.InsertEntry("new_section", "key1", 5); // Create new entry
    inih::INIWriter::write("output.ini", r); // Dump ini to file

    return 0;
}
```

To learn more, please refer to [test folder](https://github.com/SSARCandy/ini-cpp/tree/master/test), it covered ALL utilities.

## Install

Simply copy the header file `ini/ini.h` to your project, then done. 
