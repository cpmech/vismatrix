#ifndef CPP_UTIL_STRING_HPP_
#define CPP_UTIL_STRING_HPP_

#include <string>
#include <utility>

#include <util/string.h>

namespace util
{
    std::pair<std::string, std::string> split_filename(std::string filename)
    {
        using namespace std;
        // look at the extension...
	    typedef string::size_type position;
    	
	    position dot = filename.find_last_of(".");
    	
	    if (dot != string::npos)
	    {
		    return make_pair(filename.substr(0,dot),filename.substr(dot+1));
        }
        else
        {
            return make_pair(filename, std::string());
        }
    }
    
    namespace impl
    {
        // this is a workaround for the world's most ridiculous portability
        // bug between gcc and msvc.
        int lower_case ( int c )
        {
            return tolower ( c );
        }
    } 

    /**
     * Convert a string to lowercase and return a new string
     */
    std::string to_lowercase(std::string& str)
    {
        std::string rval(str);
        lowercase(rval);
        return rval;
    }
    
    /**
     * Convert a string to lowercase inplace
     */
    std::string& lowercase(std::string& str)
    {
        transform(str.begin(),str.end(),
            str.begin(), impl::lower_case);
    }
}

#endif /* CPP_UTIL_STRING_HPP_ */

