/*
autohor : zhengchong@tj.ia.ac.cn
time 2017/11/20 
I define smoe base Data type at here and every can add more Data type.but i wish 
that we all working at the same identify mentality name rule.
such as:  struct xxx_s{};  or  typedef struct xxx_s{}xxx_t;  

*/
namespace baseservice
{
	typedef struct baseKV_s
    {
    public:
        baseKV_s();
        ~baseKV_s();
    	void *data;
        long  len;
    }baseKV_t;
	typedef struct memKV_s
    {
    public:
        memKV_s();
        ~memKV_s();
    	void *buf;
        long  len;
        long bufferlen;
    }memKV_t;
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
