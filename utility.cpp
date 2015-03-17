#include <cstdlib>
#include <cstdio>
#include <string>
#include "utility.h"

void error(std::string msg)
{
    perror(msg.c_str());
    abort();
    return;
}
