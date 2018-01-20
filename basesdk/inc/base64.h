#ifndef __BASE64_H__
#define __BASE64_H__


#include <stdio.h>
#include <string.h>
namespace baseservice{
extern int base64encode(char *out, char *in, int length);
extern int base64decode(char *out, char *in, int length);
}


#endif 
