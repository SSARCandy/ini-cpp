# C++ INI Parser

Yet another `.ini` parser for modern c++ (made for cpp17), inspired by [inih](https://github.com/benhoyt/inih)


## Example

```cpp
#include "ini/INIReader.h"
using namespace inireader;

int main() {
    INIReader r{"./config.ini"};

    const std::string v = r.Get<std::string>("section1", "any");
    const int v = r.Get<int>("section1", "any");
    const float v = r.Get<float>("section1", "any");
    const double v = r.Get<double>("section1", "any");
    const long v = r.Get<long>("section1", "any");

    return 0;
}
```

## Install

Simply copy the header file `ini/INIReader.h` to your project, then done. 
