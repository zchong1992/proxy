 /*!
* \file baseDataType.h
* \brief 用于基本类型定义
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/
namespace baseservice
{
/**
* \brief baseKV_s kv数据结构,被其他接口调用
*/
	typedef struct baseKV_s
    {
    public:
        baseKV_s();
        ~baseKV_s();
    	void *data;
        long  len;
    }baseKV_t;

/**
* \brief memKV_s 内存kv数据结构,被其他接口调用
*/
	typedef struct memKV_s
    {
    public:
        memKV_s();
        ~memKV_s();
    	void *buf;
        long  len;
        long bufferlen;
    }memKV_t;
/**
* \brief baseImg_s 图像数据结构,被其他接口调用
*/
    typedef struct baseImg_s
    { 
    /*
    data can be read by members,but update only by function
    */
    public:
    	baseImg_s();
    	baseImg_s(const struct baseImg_s&A);
    	~baseImg_s();
        const struct baseImg_s& copy(const struct baseImg_s&A);
        const struct baseImg_s &operator=(const struct baseImg_s&A);
        void setData(const void *data,long width,long heigth,long channels,long widthStep=0);
        void Realease();
        unsigned char*data;
        unsigned long width;
        unsigned long height;
        unsigned long channels;
        unsigned long widthStep;
    }baseImg_t;
	
}
