
#include "libbaseservice.h"
using namespace baseservice;
void lib_init()
{
    znlog::Init();
    znheartbeat::Init();
}

