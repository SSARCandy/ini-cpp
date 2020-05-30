// Read an INI file into easy-to-access name/value pairs.

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// https://github.com/benhoyt/inih
/* inih -- simple .INI file parser

inih is released under the New BSD license (see LICENSE.txt). Go to the project
home page for more info:

https://github.com/benhoyt/inih

*/

#ifndef __INI_H__
#define __INI_H__

#include <stdio.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <vector>
#include <type_traits>
#include <sstream>
#include <iterator>
#include <unordered_map>

namespace inireader {

/* Typedef for prototype of handler function. */
typedef int (*ini_handler)(void* user, const char* section,
                           const char* name, const char* value);

/* Typedef for prototype of fgets-style reader function. */
typedef char* (*ini_reader)(char* str, int num, void* stream);


#define INI_STOP_ON_FIRST_ERROR 1
#define INI_MAX_LINE 200
#define MAX_SECTION 50
#define MAX_NAME 50
#define INI_INLINE_COMMENT_PREFIXES ";"



/* Strip whitespace chars off end of given string, in place. Return s. */
inline static char* rstrip(char* s)
{
    char* p = s + strlen(s);
    while (p > s && isspace((unsigned char)(*--p)))
        *p = '\0';
    return s;
}

/* Return pointer to first non-whitespace char in given string. */
inline static char* lskip(const char* s)
{
    while (*s && isspace((unsigned char)(*s)))
        s++;
    return (char*)s;
}

/* Return pointer to first char (of chars) or inline comment in given string,
   or pointer to null at end of string if neither found. Inline comment must
   be prefixed by a whitespace character to register as a comment. */
inline static char* find_chars_or_comment(const char* s, const char* chars)
{
    int was_space = 0;
    while (*s && (!chars || !strchr(chars, *s)) &&
           !(was_space && strchr(INI_INLINE_COMMENT_PREFIXES, *s))) {
        was_space = isspace((unsigned char)(*s));
        s++;
    }
    return (char*)s;
}

/* Version of strncpy that ensures dest (size bytes) is null-terminated. */
inline static char* strncpy0(char* dest, const char* src, size_t size)
{
    strncpy(dest, src, size);
    dest[size - 1] = '\0';
    return dest;
}

/* See documentation in header file. */
inline int ini_parse_stream(ini_reader reader, void* stream, ini_handler handler,
                     void* user)
{
    char* line;
    char section[MAX_SECTION] = "";
    char prev_name[MAX_NAME] = "";
    char* start;
    char* end;
    char* name;
    char* value;
    int lineno = 0;
    int error = 0;

    line = (char*)malloc(INI_MAX_LINE);
    if (!line) {
        return -2;
    }

    /* Scan through stream line by line */
    while (reader(line, INI_MAX_LINE, stream) != NULL) {
        lineno++;

        start = line;
        if (lineno == 1 && (unsigned char)start[0] == 0xEF &&
                           (unsigned char)start[1] == 0xBB &&
                           (unsigned char)start[2] == 0xBF) {
            start += 3;
        }
        start = lskip(rstrip(start));

        if (*start == ';' || *start == '#') {
            /* Per Python configparser, allow both ; and # comments at the
               start of a line */
        }
        else if (*start == '[') {
            /* A "[section]" line */
            end = find_chars_or_comment(start + 1, "]");
            if (*end == ']') {
                *end = '\0';
                strncpy0(section, start + 1, sizeof(section));
                *prev_name = '\0';
            }
            else if (!error) {
                /* No ']' found on section line */
                error = lineno;
            }
        }
        else if (*start) {
            /* Not a comment, must be a name[=:]value pair */
            end = find_chars_or_comment(start, "=:");
            if (*end == '=' || *end == ':') {
                *end = '\0';
                name = rstrip(start);
                value = lskip(end + 1);
                end = find_chars_or_comment(value, NULL);
                if (*end)
                    *end = '\0';
                rstrip(value);

                /* Valid name[=:]value pair found, call handler */
                strncpy0(prev_name, name, sizeof(prev_name));
                if (!handler(user, section, name, value) && !error)
                    error = lineno;
            }
            else if (!error) {
                /* No '=' or ':' found on name[=:]value line */
                error = lineno;
            }
        }

        if (error)
            break;
    }

    return error;
}

inline int ini_parse_file(FILE* file, ini_handler handler, void* user)
{
    return ini_parse_stream((ini_reader)fgets, file, handler, user);
}

inline int ini_parse(const char* filename, ini_handler handler, void* user)
{
    FILE* file;
    int error;

    file = fopen(filename, "r");
    if (!file)
        return -1;
    error = ini_parse_file(file, handler, user);
    fclose(file);
    return error;
}

#endif /* __INI_H__ */


#ifndef __INIREADER_H__
#define __INIREADER_H__


// Read an INI file into easy-to-access name/value pairs. (Note that I've gone
// for simplicity here rather than speed, but it should be pretty decent.)
class INIReader
{
public:
    // Empty Constructor
    INIReader() {};

    // Construct INIReader and parse given filename. See ini.h for more info
    // about the parsing.
    INIReader(std::string filename);

    // Construct INIReader and parse given file. See ini.h for more info
    // about the parsing.
    INIReader(FILE *file);

    // Return the result of ini_parse(), i.e., 0 on success, line number of
    // first error on parse error, or -1 on file open error.
    int ParseError() const;

    // Return the list of sections found in ini file
    const std::set<std::string>& Sections() const;

    template<typename T>
    T Get(std::string section, std::string name) const;

    template<typename T>
    std::vector<T> GetVector(std::string section, std::string name) const;

protected:
    int _error;
    std::map<std::string, std::string> _values;
    std::set<std::string> _sections;
    static std::string MakeKey(std::string section, std::string name);
    static int ValueHandler(void* user, const char* section, const char* name, const char* value);
};

#endif  // __INIREADER_H__


#ifndef __INIREADER__
#define __INIREADER__


inline INIReader::INIReader(std::string filename)
{
    _error = ini_parse(filename.c_str(), ValueHandler, this);
}

inline INIReader::INIReader(FILE *file)
{
    _error = ini_parse_file(file, ValueHandler, this);
}

inline int INIReader::ParseError() const
{
    return _error;
}

inline const std::set<std::string>& INIReader::Sections() const
{
    return _sections;
}

template<typename T>
inline T INIReader::Get(std::string section, std::string name) const {
    std::string key = MakeKey(section, name);
    if (!_values.count(key)) {
        throw std::runtime_error("key " + key + " not found.");
    }
    
    if constexpr (std::is_same<T, int>()) {
        return std::stoi(_values.at(key));
    } else if constexpr (std::is_same<T, float>()) {
        return std::stof(_values.at(key));
    } else if constexpr (std::is_same<T, double>()) {
        return std::stod(_values.at(key));
    } else if constexpr (std::is_same<T, long>()) {
        return std::stol(_values.at(key));
    } else if constexpr (std::is_same<T, unsigned long>()) {
        return std::stoul(_values.at(key));
    } else if constexpr (std::is_same<T, std::string>()) {
        return _values.at(key);
    } else if constexpr (std::is_same<T, bool>()) {
        std::string s{_values.at(key)};
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
    
        const std::unordered_map<std::string, bool> s2b{
            {"true", true}, {"yes", true}, {"on", true},
            {"false", false}, {"no", false}, {"off", false},
        };
        return s2b.find(s)->second;
    } else {
        throw std::runtime_error("unsupport type.");
    }
}

template<typename T>
inline std::vector<T> INIReader::GetVector(std::string section, std::string name) const {
    std::string key = MakeKey(section, name);
    if (!_values.count(key)) {
        throw std::runtime_error("key " + key + " not found.");
    }

    std::istringstream iss(_values.at(key));
    const std::vector<T> vs{std::istream_iterator<T>{iss}, std::istream_iterator<T>()};
    return vs;
}

inline std::string INIReader::MakeKey(const std::string section, const std::string name)
{
    std::string key = section + "=" + name;
    // Convert to lower case to make section/name lookups case-insensitive
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    return key;
}

inline int INIReader::ValueHandler(void* user, const char* section, const char* name, const char* value)
{
    INIReader* reader = (INIReader*)user;
    std::string key = MakeKey(section, name);
    if (reader->_values[key].size() > 0)
        reader->_values[key] += "\n";
    reader->_values[key] += value;
    reader->_sections.insert(section);
    return 1;
}

#endif  // __INIREADER__
}