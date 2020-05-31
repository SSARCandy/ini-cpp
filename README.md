# C++ INI Parser

[![C/C++ CI](https://github.com/SSARCandy/ini-cpp/workflows/C/C++%20CI/badge.svg?branch=master)](https://github.com/SSARCandy/ini-cpp/actions)
[![Build Status](https://travis-ci.org/SSARCandy/ini-cpp.svg?branch=master)](https://travis-ci.org/SSARCandy/ini-cpp)
[![codecov](https://codecov.io/gh/SSARCandy/ini-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/SSARCandy/ini-cpp)
  
Yet another `.ini` parser for modern c++ (made for cpp17), inspired and extend from [inih](https://github.com/benhoyt/inih).


## Example

```cpp
#include "ini/INIReader.h"

int main() {
    inih::INIReader r{"./test/fixtures/config.ini"};

    const auto& v1 = r.Get<std::string>("section1", "any"); // "5"
    const auto& v2 = r.Get<int>("section1", "any"); // 5
    const auto& v3 = r.Get<double>("section1", "any"); // 5.0
    const auto& v4 = r.GetVector<float>("section2", "any_vec"); // [1.0, 2.0, 3.0]
    const auto& v5 = r.GetVector<std::string>("section2", "any_vec"); // ["1", "2", "3"]

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

To learn more, please refer to [test folder](https://github.com/SSARCandy/ini-cpp/tree/master/test), it covered ALL utilities.

## Install

Simply copy the header file `ini/INIReader.h` to your project, then done. 
