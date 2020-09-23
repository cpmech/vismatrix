#ifndef CPP_UTIL_FILE_H_
#define CPP_UTIL_FILE_H_

#include <string>
#include <fstream>
#include <cctype>

#ifndef CPP_UTIL_FILE_DATA_CHECK_SIZE 
#define CPP_UTIL_FILE_DATA_CHECK_SIZE 1024
#endif /* CPP_UTIL_FILE_DATA_CHECK_SIZE */

namespace util
{
    /*
     * exists wrappers
     */
    bool file_exists(const std::string& filename);

    /**
     * Enumerate a small set of simple filetypes
     */
    enum filetypes
    {
        filetype_binary=1,
        filetype_text,
        filetype_numeric_text
    };

    filetypes guess_filetype(const char* filename);
    filetypes guess_filetype(const std::string& filename);
    std::streamsize filesize(std::istream &s);
    std::streamsize filesize(const char* filename);
    std::streamsize filesize(const std::string& filename);
    bool gzip_header(std::istream& s);
    bool gzip_header(const char* filename);
    bool gzip_header(const std::string& filename);
}


#endif /* CPP_UTIL_FILE_H_ */

