#ifndef CPP_UTIL_STRING_H_
#define CPP_UTIL_STRING_H_

#include <string>
#include <utility>

namespace util
{
    std::pair<std::string, std::string> split_filename(std::string& filename);
    std::string to_lowercase(std::string& str);
    std::string& lowercase(std::string& str);
}

#endif /* CPP_UTIL_STRING_H_ */

