
 /*!
* \file img.h
* \brief 为图像添加头部信息,可以以文件流的方式,存在于内存中,用于对android通讯的加速
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/
#ifndef __IMG_H__
#define __IMG_H__

#include "global.h"
#include "log.h"

namespace baseservice{
    
/**
* \brief 图像操作类,生成头部信息和jpeg转码操作
*/
     class zkznImg
    {
    public:
    
/**
* \brief jpeg转码操作
*/
        static int jpeg2buf(ZUINT1 *dst,ZINT4 *buflen,ZUINT1 * inbuf,ZINT4 width ,ZINT4 height,ZINT4 channels,ZINT4 quality);
            
/**
* \brief 图像操作类,生成bmp头部信息
*/
        static int  createbmpheader(unsigned char *buf,int *length,const int width,const int height,const int channels);
     };
}

#endif






