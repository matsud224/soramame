﻿#pragma once

#if (_WIN32 || _WIN64)

#define RESET   ""
#define BLACK   ""      /* Black */
#define RED     ""      /* Red */
#define GREEN   ""      /* Green */
#define YELLOW  ""      /* Yellow */
#define BLUE    ""      /* Blue */
#define MAGENTA ""      /* Magenta */
#define CYAN    ""      /* Cyan */
#define WHITE   ""      /* White */
#define BOLDBLACK   ""      /* Bold Black */
#define BOLDRED     ""      /* Bold Red */
#define BOLDGREEN   ""      /* Bold Green */
#define BOLDYELLOW  ""      /* Bold Yellow */
#define BOLDBLUE    ""      /* Bold Blue */
#define BOLDMAGENTA ""      /* Bold Magenta */
#define BOLDCYAN    ""      /* Bold Cyan */
#define BOLDWHITE   ""      /* Bold White */

#define BG_BLACK   ""      /* Black */
#define BG_RED     ""      /* Red */
#define BG_GREEN   ""      /* Green */
#define BG_YELLOW  ""      /* Yellow */
#define BG_BLUE    ""      /* Blue */
#define BG_MAGENTA ""      /* Magenta */
#define BG_CYAN    ""      /* Cyan */
#define BG_WHITE   ""      /* White */

#else

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#define BG_BLACK   "\033[40m"      /* Black */
#define BG_RED     "\033[41m"      /* Red */
#define BG_GREEN   "\033[42m"      /* Green */
#define BG_YELLOW  "\033[43m"      /* Yellow */
#define BG_BLUE    "\033[44m"      /* Blue */
#define BG_MAGENTA "\033[45m"      /* Magenta */
#define BG_CYAN    "\033[46m"      /* Cyan */
#define BG_WHITE   "\033[47m"      /* White */

#endif