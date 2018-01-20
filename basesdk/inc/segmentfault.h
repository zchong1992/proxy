
#ifndef __SEGMENTFAULT_H__
#define __SEGMENTFAULT_H__
#include "global.h"
#include "log.h"
namespace baseservice{
class znsegfault_catch
{
     public :
        static char handlesvaule[100][20];
        static void handle();
        static void catch_sign(void);
        static void no_print_seg_frame(int signal);
        static void print_seg_frame(int signal);
};



};
#endif 

