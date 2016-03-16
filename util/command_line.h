#ifndef CPP_UTIL_COMMAND_LINE_H_
#define CPP_UTIL_COMMAND_LINE_H_

/*
 * David Gleich
 * 21 January, 2007
 * Copyright, Stanford University 2007
 */
 
/**
 * @file command_line.h
 * Simple header file for the command_line parsing utilities 
 * in the c++ util library 
 */

#include <string>
#include <vector>

namespace util
{
    void split_command_line(std::string s, std::vector<std::string>& args);
    void args_to_c_argc_argv(const std::vector<std::string>& args, int& sargc, char**& sargv);
}

#endif /* CPP_UTIL_COMMAND_LINE_H_ */

