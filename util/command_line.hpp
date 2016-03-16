#ifndef CPP_UTIL_SPLIT_COMMAND_LINE_HPP_
#define CPP_UTIL_SPLIT_COMMAND_LINE_HPP_

/*
 * David Gleich
 * 21 January, 2007
 * Copyright, Stanford University 2007
 */
 
/**
 * @file command_line.hpp
 * A set of command line parsing utilities for the c++ utility
 * class.  These are not meant to replace a full-featured
 * command line package, but simple to provide a set of useful
 * utility functions that other packages may omit.
 */

#include "command_line.h"
#include <cctype>
#include <string.h>

namespace util
{


/**
 * split_command_line takes a command line as a string and implements
 * the a simple parsing of the command line into an argument array
 * 
 * The contents of the vector are left intact, so for correct
 * command line parsing, they should be elimianted before 
 * calling this function (unless you want to add a series of
 * additional options).
 *
 * @param cmdline a pointer to a null-terminated cmdline string, this
 * parameter is modified by this call.
 * @param args a vector that will contain all of the arguments
 */
void split_command_line(std::string s, std::vector<std::string>& args)
{
    bool in_quotes = false;
    bool is_literal = false;
    std::string param;
    
    // iterate over all characters of the string
    for (std::string::const_iterator i = s.begin(); i != s.end(); ++i)
    {
        if (*i == '"' && is_literal == false) {
            // if we enter a quoted region where the next character is 
            // non-literal then exit the quoted portion
            in_quotes = !in_quotes;
        }
        else 
        {
            // all characters in this region must be non-literal
            if (std::isspace(*i) && in_quotes == false) 
            {
                // we encounted a space, so we should clear the current
                // parameter if it has any content
                if (param.empty() == false) {
                    args.push_back(param);
                    param.erase();
                }
            }
            else if (*i == '\\' && in_quotes == true && is_literal == false) {
                // if we encounter a \ character, it may be 
                // trying to encode \" or \\ which would indicate
                // something we need to preserve, but this
                // can only be true inside a string
                is_literal = true;
            }
            else {
                // at this point, we cannot have a literal character anymore.
                is_literal = false;
                
                param += *i;
            }
        }
    }
    
    // check to see if we added the final param
    if (!param.empty()) { 
        args.push_back(param); 
        param.erase();
    } 
}

/**
 * This function takes a vector of arguments and creates a
 * c-style set of arguments.
 * 
 * The caller is responsible for freeing the memory allocated
 * by calling
 * 
 * delete[] sargv[0];
 * delete[] sargv;
 * 
 * The first command will remove the memory for all 
 * of the string parameters (which are all allocated in one memory block)
 * The second command will remove the memory for all the nested 
 * pointers.
 * 
 * @param args the vector of arguments to convert to c-style arrays
 * @param sargv a reference to the future 
 */
void args_to_c_argc_argv(const std::vector<std::string>& args, int& sargc, char**& sargv)
{
    sargc = (int)args.size();
    
    int total_len = 0;
    
    typedef std::vector<std::string>::size_type index_type;
    for (index_type i = 0; i < args.size(); ++i) {
        total_len += (int)(args[i].length() + 1);
    }
    
    // allocate the memory for the output
    sargv = new char*[sargc];
    sargv[0] = new char[total_len];
    
    char *ptr = sargv[0];
    
    // create the output
    for (index_type i = 0; i < args.size(); ++i) {
        sargv[i] = ptr;
        strncpy(sargv[i],args[i].c_str(),args[i].length());
        sargv[i][args[i].length()] = '\0';
        
        // increment the pointer to the next possible location
        ptr += args[i].length() + 1;
    }
} 

}

#endif /* CPP_UTIL_SPLIT_COMMAND_LINE_HPP_ */

