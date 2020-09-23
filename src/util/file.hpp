#ifndef CPP_UTIL_FILE_HPP_
#define CPP_UTIL_FILE_HPP_

#include <string>
#include <fstream>
#include <cctype>

#include <util/file.h>

#ifndef CPP_UTIL_FILE_DATA_CHECK_SIZE 
#define CPP_UTIL_FILE_DATA_CHECK_SIZE 1024
#endif /* CPP_UTIL_FILE_DATA_CHECK_SIZE */

namespace util
{
    /*
     * exists wrappers
     */
    bool file_exists(const std::string& filename);
    filetypes guess_filetype(const char* filename);

    /**
     * Wrapper function for a std::string
     */
    filetypes guess_filetype(const std::string& filename)
    { return guess_filetype(filename.c_str()); }

    /*
     * filesize wrappers
     */
    std::streamsize filesize(std::istream &s);

    /**
     * Wrapper function for a const char*
     */
    std::streamsize filesize(const char* filename)
    { 
        std::ifstream f(filename, std::ios::binary | std::ios::in);
        return filesize(f); 
    }

    /**
     * Wrapper function for a std::string.
     */
    std::streamsize filesize(const std::string& filename)
    { return filesize(filename.c_str()); }
    

    /*
     * gzip_header wrappers
     */
    // prototype
    bool gzip_header(std::istream& s);

    /**
     * Wrapper function to check for the gzip header of a filename
     */
    bool gzip_header(const char* filename)
    { 
        std::ifstream f(filename, std::ios::binary | std::ios::in);
        return gzip_header(f); 
    }

    /**
     * Wrapper function for a std::string.
     */
    bool gzip_header(const std::string& filename)
    { return gzip_header(filename.c_str()); }
}

/**
 * Test if a file exists.
 *
 * @filename the name of the file
 * @return true if the file exists, false otherwise
 */
bool util::file_exists(const std::string& filename)
{
    using namespace std;
    ifstream t(filename.c_str());
    t.close();
    if (t.fail()) { return false; }
    else { return true; }
}

/**
 * Guess the filetype by reading some of the data and trying to 
 * determine if the file is 
 * i) binary
 * ii) text
 * iii) numeric text 
 *
 * The algorithm employed is fairly simple, it reads 
 * CPP_UTIL_FILE_DATA_CHECK_SIZE byes from the file and processes the data
 * to look for byte patterns not allowed by the various formats.  The
 * binary format is the fall back in case none of the others match.  That is,
 * we declare a file binary if it is not a text file, and not a numeric
 * text file.
 *
 * <b>NB</b> At the moment, the code only determines rudimentary text
 * files not encoded with any of the unicode formats.  
 *
 * @param filename the name of the file to test
 * @return an enum covering the possible filetypes
 */
util::filetypes util::guess_filetype(const char* filename)
{
    using namespace std;

    char data[CPP_UTIL_FILE_DATA_CHECK_SIZE];

    streamsize data_size = CPP_UTIL_FILE_DATA_CHECK_SIZE;

    // read data_check_size bytes from the file
    {
        ifstream f(filename, ios::binary | ios::in);
        data_size = f.readsome(data, data_size);
    }

    filetypes rval = util::filetype_numeric_text;

    // count the number of characters with the high_bit set.
    int high_bit_count=0;

    for (int i = 0; i < data_size; i++)
    {
        char c = data[i];
        if (rval == util::filetype_numeric_text)
        {
            if (!isspace(c) && !isdigit(c) && c != 'e' && c != '-' && c != '+')
            {
                rval = util::filetype_text;
            }
        }
        if (rval == util::filetype_text)
        {
            // no text file allows these characters
            if (c >= 0x00 && c <= 0x10)
            {
                rval = util::filetype_binary;
            }
            else if (c < 0)
            {
                // if c < 0, then the high bit is set
                high_bit_count++;
            }
        }
        if (rval == util::filetype_binary)
        {
            // if the file type is binary, then there is nothing
            // else we say about it
            break;
        }
    }

    if (rval == util::filetype_text)
    {
        // if the number of bytes with the high_bit set is large, then 
        // declare the file as binary
        if (high_bit_count*100/data_size > 25) { return (util::filetype_binary); }
        else { return (rval); }
    }

    return (rval);
}

/**
 * Compute the filesize of a stream.  The stream must be seekable in order
 * for this operation to succeed successfully.
 *
 * @param s the seekable stream
 * @return the total size of a stream
 */
std::streamsize util::filesize(std::istream &s)
{
    using namespace std;
    istream::off_type cur = s.tellg();
    s.seekg (0, ios::beg);
    istream::off_type begin = s.tellg();
	s.seekg (0, ios::end);
	istream::off_type end = s.tellg();
    s.seekg (cur, ios::beg);

    return (end - begin);
}

/**
 * Check for a gzip header on a file.  The stream must be at the 
 * position where the header is expected.
 *
 * The gzip header is defined by the two bytes 
 * \x1f \x8b
 * starting the file.
 *
 * @param s the stream itself
 * @return true if the gzip header is present, false otherwise
 */

bool util::gzip_header(std::istream& s)
{
    const int gzip_header_size = 2;
    char header[gzip_header_size];

    std::streamsize data_size = gzip_header_size;

    // read the header bytes from the file
    data_size = s.readsome(header, data_size);

    if (header[0] == '\x1f' && header[1] == '\x8b') { return true; }
    else { return false; }
}

#endif /* CPP_UTIL_FILE_HPP_ */

