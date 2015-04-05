#include <cstdlib>
#include <iostream>
#include <string>
#include "utility.h"
#include "lexer.h"


void error(std::string msg)
{
    cerr<<msg<<endl;
    abort();
    return;
}
