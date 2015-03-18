#include <cstdlib>
#include <cstdio>
#include <string>
#include "utility.h"

void error(std::string msg)
{
    printf(msg.c_str());
    abort();
    return;
}
