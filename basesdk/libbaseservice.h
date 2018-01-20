

#ifndef __LIBBASESERVICE_H__
#define __LIBBASESERVICE_H__



#include "./inc/config.h"
#include "./inc/heartbeat.h"
#include "./inc/log.h"
#include "./inc/segmentfault.h"
#include "./inc/object.h"
#include "./inc/img.h"
#include "./inc/aes.h"
#include "./inc/base64.h"
#include "./inc/pointer.h"
#include "./inc/dir.h"
namespace baseservice
{

#ifdef WIN32
#define TIME_T time_t
#define GET_TIME(a) (*a)=clock()
#define CAL_TIME(a,b) ((*a)-(*b))
#else
#include "sys/time.h"
#define TIME_T timeval
#define GET_TIME(a) gettimeofday((a),0);
#define CAL_TIME(a,b) (((*a).tv_sec-(*b).tv_sec)*1000+((*a).tv_usec-(*b).tv_usec)/1000)
#endif

void lib_init();

}


#endif




