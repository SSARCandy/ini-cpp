# C++ INI Parser

[![C/C++ CI](https://github.com/SSARCandy/ini-cpp/actions/workflows/c-cpp.yml/badge.svg)](https://github.com/SSARCandy/ini-cpp/actions/workflows/c-cpp.yml)
[![codecov](https://codecov.io/gh/SSARCandy/ini-cpp/branch/master/graph/badge.svg)](https://codecov.io/gh/SSARCandy/ini-cpp)
  
Yet another `.ini` parser for modern c++ (made for cpp17). It is a header only implementation, easy to install and simple to use.


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

## API

All APIs live in namespace `inih`. All errors are reported by throwing `std::runtime_error`.

### `INIReader`

| API | Description |
| --- | --- |
| `INIReader(const std::string& filename)` | Parse the given ini file; throws on missing file, syntax error (with line number) or duplicate keys |
| `INIReader(FILE* file)` | Same, but reads from an already-opened `FILE*` (the caller keeps ownership) |
| `Sections()` | All section names, as `std::set<std::string>` |
| `Keys(section)` | All key names in the given section, as `std::set<std::string>` |
| `Get(section)` | The whole section, as `std::unordered_map<std::string, std::string>` |
| `Get<T = std::string>(section, name)` | The value parsed as `T`; throws if section/key is missing or the value cannot be parsed |
| `Get<T>(section, name, default_value)` | Same, but returns `default_value` instead of throwing |
| `GetVector<T = std::string>(section, name)` | The whitespace-separated value parsed as `std::vector<T>` |
| `GetVector<T>(section, name, default_vector)` | Same, but returns `default_vector` instead of throwing |
| `InsertEntry(section, name, value)` | Add a new entry (scalar or `std::vector`); throws if the key already exists |
| `UpdateEntry(section, name, value)` | Overwrite an existing entry (scalar or `std::vector`); throws if the key does not exist |

`T` can be `std::string`, `bool` (accepts `1/0/true/false/yes/no/on/off`, case-insensitive), any integer or floating-point type, or any custom type readable from a stream with `operator>>`.

### `INIWriter`

| API | Description |
| --- | --- |
| `INIWriter::write(filepath, reader, overwrite = false)` | Dump an `INIReader`'s content to a file; unless `overwrite` is true, throws if the file already exists |

## Install

### Manual

Simply copy the header file `ini/ini.h` to your project, then done.

### vcpkg

ini-cpp is in the [vcpkg registry](https://github.com/microsoft/vcpkg/tree/master/ports/ini-cpp):

```bash
vcpkg install ini-cpp
```

It is header-only, so consume it from CMake via `find_path`:

```cmake
find_path(INI_CPP_INCLUDE_DIRS "ini/ini.h")
target_include_directories(main PRIVATE ${INI_CPP_INCLUDE_DIRS})
```
