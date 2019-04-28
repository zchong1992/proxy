

#include <iostream>
#include <string>
#include "function.h"
#include "com_struct.h"

using namespace baseservice;
using namespace std;
void printHelp()
{
    printf("");
}

void initLog()
{
    znlog::getInstance()->Init();
    znlog::getInstance()->set_level(ZLOGINFO, ZLOGINFO);
    znlog::getInstance()->set_log_file("server_online_log.txt");
}
#ifndef TEST_CODE
int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printHelp();
        return 0;
    }
    initLog();
    string type = argv[1];
    if (type == "online")
    {
        on_main(argc - 1, &argv[1]);
    }
    else if (type == "offline")
    {
        off_main(argc - 1, &argv[1]);
    }
    else
    {
        printHelp();
    }

    return 0;
}
#endif