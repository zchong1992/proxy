
 /*!
* \file msgTransport.h
* \brief 消息处理基类,用于网络格式化传输,文件保存,跨线程数据传输时使用,部分线程安全
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/

#ifndef __MSG_TRANSPORT_H__
#define __MSG_TRANSPORT_H__
#include "global.h"
#include "cThread.h"
#include "zlibnet.h"
namespace baseservice
{
     
/**
* \brief 错误类
*/
class ZERROR
{
  public:
    ZERROR(std::string str) : errInfo(str) { errType = 0; };
    ZERROR(std::string str, int type) : errInfo(str), errType(type){};
    const std::string what(){return errInfo;}
    std::string errInfo;
    int errType;

};
     
/**
* \brief 消息封装类
*/
class Message
{
  public:
    enum
    {
        SUCCESS = 0,
        VAILD_TYPE = -999,
        VAILD_LEN,
        VAILD_DATA,
        VAILD_MAGIC,
        VAILD_BUFFER,
        VAILD_ALLOC_FAIL,
        VAILD_DUP_TYPE,
        VAILD_OTHER
    };

  public:
    static int writeHeader(void *buf, int size, char magic[], unsigned int type, unsigned int DataLen);
    static int readHeader(const void *buf, int size, char magic[], unsigned int &type, unsigned int &DataLen);
    Message();
    virtual ~Message();
    Message(const Message &msg);
    void copy(const Message &msg);
    Message & operator=(const Message&);
    void *getData();
    int getLen();
    int getType();
    int setData2Buffer(void *buf, int bufLen);
    int getDataFromBuffer(const void *buf, int bufLen);
    int setType(int type);
    int setMagic(const char magic[4]);
    std::string getMagic();
    void release();
    int setData(const void *data, int len);
    int getSize();

  private:
    void *mPtr;
    int mLen;
    int mType;
    char mMagic[4];
    LOCKER mLocker;
};
 
/**
* \brief 消息读取类,从buf中读取一组消息
*/
class MessageReader
{
  public:
    MessageReader();
    int getSize();
    int getDataFromBuffer(const void *buf, int bufLen);
    void release();
    typedef std::vector<Message *> MessageQueue;
    MessageQueue mMQ;

  private:
};

 
/**
* \brief 消息写入类,将一组消息写入一个buf
*/
class MessageWriter
{
  public:
    int setData2Buffer(void *buf, int bufLen);
    typedef std::vector<Message *> MessageQueue;
    MessageQueue mMQ;
};

/**
* \brief 消息处理类,可以在MessageMananger中注册对应type类型的消息处理函数,需要重载dealMsg函数
*/
class MessageWorker
{
  public:
    MessageWorker(int type);
    virtual ~MessageWorker();
    virtual int dealMsg(void *buf, int len);

  private:
    MessageWorker();
    int type;
};

/**
* \brief 消息接收处理类,目前未完成
*/
class MessageMananger : public cThread
{
  public:
    virtual int registWorker(MessageWorker *);
    virtual int run();

    typedef std::vector<MessageWorker *> WokerQueue;
    typedef std::vector<Message *> MessageQueue;

  private:
    WokerQueue mWQ_;
    MessageQueue mMQ;
    LOCKER mWQL, mMQL;
};
}; // namespace baseservice

#endif
