#include "msgTransport.h"

using namespace std;
namespace baseservice
{
// enum {
//     SUCCESS=0,
//     VAILD_TYPE=-999,
//     VAILD_LEN,
//     VAILD_DATA,
//     VAILD_MAGIC,
//     VAILD_ALLOC_FAIL,
//     VAILD_DUP_TYPE,
//     VAILD_OTHER
//     };
//     public:
#define MESSAGE_HEAD_LEN 10
Message::Message()
{
    const char *ptr = "IRST";
    memcpy(mMagic, ptr, 4);
    mType = 0;
    mLen = 0;
    mPtr = 0;
    PTHRAED_INIT(&mLocker);
}

Message::Message(const Message &msg)
{
    Message();
    copy(msg);
}
Message::~Message()
{
    release();
}
Message & Message::operator=(const Message&msg)
{
    copy(msg);
    return *this;
}
void Message::copy(const Message &msg)
{
    if (mLen != msg.mLen)
    {
        if (mPtr)
        {
            free(mPtr);
            mPtr = 0;
        }
        mLen=0;
        memcpy(mMagic,"IRST",4);
        mType=0;
    }
    if(msg.mLen==0)
        return;
    mPtr=malloc(msg.mLen+1);
    memcpy(mPtr,msg.mPtr,msg.mLen);
    mLen=msg.mLen;
    char* ptr=(char*)mPtr;
    ptr[mLen]=0;
    memcpy(mMagic,msg.mMagic,4);
    mType=msg.mType;

}
void Message::release()
{
    PTHREAD_LOCK(&mLocker);
    if (mPtr)
    {
        free(mPtr);
    }
    mPtr = 0;
    mLen = 0;
    mType = 0;
    PTHREAD_UNLOCK(&mLocker);
}
int Message::writeHeader(void *buf, int size, char magic[],
                         unsigned int type, unsigned int dataLen)
{
    if (buf == 0 || size < MESSAGE_HEAD_LEN)
        return VAILD_LEN;
    memcpy(buf, magic, 4);
    char *ptr = (char *)buf;
    *(unsigned short *)(ptr + 4) = ntohs(type);
    *(unsigned int *)(ptr + 6) = ntohl(dataLen);
    return MESSAGE_HEAD_LEN;
}

int Message::readHeader(const void *buf, int size, char magic[],
                        unsigned int &type, unsigned int &dataLen)
{
    if (buf == 0 || size < MESSAGE_HEAD_LEN)
        return VAILD_LEN;
    memcpy(magic, buf, 4);
    memcpy((char *)&type, ((char *)buf) + 4, 2);
    memcpy((char *)&dataLen, ((char *)buf) + 6, 4);
    type = ntohs(type);
    dataLen = ntohl(dataLen);
    return MESSAGE_HEAD_LEN;
}
void *Message::getData()
{
    return mPtr;
}
int Message::getLen()
{
    return mLen;
}
int Message::getType()
{
    return mType;
}
string Message::getMagic()
{
    char buf[5] = {0};
    memcpy(buf, mMagic, 4);
    string str = buf;
    return str;
}
int Message::setData2Buffer(void *buf, int bufLen)
{
    int ret = SUCCESS;
    PTHREAD_LOCK(&mLocker);
    if (buf == 0)
    {
        ret = VAILD_BUFFER;
        goto END;
    }
    if (bufLen < MESSAGE_HEAD_LEN + mLen)
    {
        ret = VAILD_LEN;
        goto END;
    }
    if (mPtr == 0)
    {
        ret = VAILD_DATA;
        goto END;
    }
    ret = writeHeader(buf, bufLen, mMagic, mType, mLen);
    if (ret < 0)
    {
        goto END;
    }
    memcpy((char *)buf + MESSAGE_HEAD_LEN, mPtr, mLen);
    ret = MESSAGE_HEAD_LEN + mLen;
END:
    PTHREAD_UNLOCK(&mLocker);
    return ret;
}
int Message::getSize()
{
    return mLen + MESSAGE_HEAD_LEN;
}
int Message::getDataFromBuffer(const void *buf, int bufLen)
{

    int ret = SUCCESS;
    char magic[4] = {0};
    unsigned int type = 0;
    unsigned int datalen = 0;
    int headLen = 0;
    if (bufLen < MESSAGE_HEAD_LEN)
    {
        ret = VAILD_LEN;
        goto END;
    }
    headLen = readHeader(buf, bufLen, magic, type, datalen);
    if (headLen < 0)
    {
        ret = headLen;
        goto END;
    }
    if (headLen != MESSAGE_HEAD_LEN)
    {
        ret = VAILD_LEN;
        goto END;
    }
    if (datalen == 0 || datalen > (100 << 20))
    {
        ret = VAILD_LEN;
        goto END;
    }
    memcpy(mMagic, magic, 4);
    mType = type;
    ret = setData(((char *)buf) + MESSAGE_HEAD_LEN, datalen);
    if (ret >= 0)

    {
        ret = MESSAGE_HEAD_LEN + mLen;
        goto END;
    }
END:
    return ret;
}
int Message::setData(const void *data, int len)
{
    int ret = SUCCESS;
    PTHREAD_LOCK(&mLocker);
    if (data == 0)
    {
        ret = VAILD_DATA;
        goto END;
    }
    if (len == 0)
    {
        ret = VAILD_LEN;
        goto END;
    }
    if (mLen != len)
    {
        if (mPtr)
            free(mPtr);
        mPtr = 0;
        mLen = 0;
    }
    if (mPtr == 0 && mLen == 0)
    {
        mPtr = malloc(len + 1);
        if (mPtr == 0)
        {
            ret = VAILD_ALLOC_FAIL;
            goto END;
        }
        memset(mPtr, 0, len + 1);
    }
    mLen = len;
    memcpy(mPtr, data, len);
    ret = SUCCESS;
END:
    PTHREAD_UNLOCK(&mLocker);
    return ret;
}
int Message::setType(int type)
{
    mType = type;
    return SUCCESS;
}
int Message::setMagic(const char magic[4])
{
    memcpy(mMagic, magic, 4);
    return SUCCESS;
}
int MessageReader::getDataFromBuffer(const void *buf, int bufLen)
{
    int pos = 0;
    int MsgLen = 0;
    int ret = 0;
    while (bufLen - pos > 10)
    {
        Message *msg = new Message();
        if (msg == 0)
        {
            char buf2[1024];
            sprintf(buf2, "alloc mem fail in %s", __FUNCTION__);
            ZERROR ze(buf2);
            throw(ze);
        }
        ret = msg->getDataFromBuffer(((char *)buf) + pos, bufLen - pos);
        if (ret <= 0)
        {
            delete msg;
            char buf2[1024];
            sprintf(buf2, "err data in %s at pos =%d bufLen=%d", __FUNCTION__, pos, bufLen);
            ZERROR ze(buf2);
            throw(ze);
        }
        MsgLen = ret;
        pos += MsgLen;
        mMQ.push_back(msg);
    }
    return pos;
}
MessageReader::MessageReader()
{
    release();
}
int MessageReader::getSize()
{
    return mMQ.size();
}
void MessageReader::release()
{
    mMQ.clear();
}
int MessageWriter::setData2Buffer(void *buf, int bufLen)
{
	return 0;
}
MessageWorker::MessageWorker(int type)
{
}

MessageWorker::~MessageWorker()
{
}

int MessageWorker::dealMsg(void *buf, int len)
{
    return 0;
}

MessageWorker::MessageWorker()
{
}

int MessageMananger::registWorker(MessageWorker *pmw)
{
    return 0;
}

int MessageMananger::run()
{
    return 0;
}
} // namespace baseservice
