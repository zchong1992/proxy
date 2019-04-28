
#include "cThread.h"

using namespace std;
using namespace baseservice;
void cThread::start()
{
    createthread(cThread::startThread,this);
};

void *cThread::startThread(void* para)
{
    cThread* ct=(cThread*)para;
    set_thread_title(ct->getThreadName().c_str());
    ct->run();
    return 0;
};

std::string cThread::getThreadName()
{
    string a="cThread";
    return a;
}