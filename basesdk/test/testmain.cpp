#include "libbaseservice.h"
#include "string.h"
using namespace std;
using namespace baseservice;
#ifdef ZWINDOWS
#pragma comment(lib,"ws2_32.lib")
#endif
void* responseClient(void * para)
{
    tcpClient client;
    client.fd=*(int*)para;
    free(para);
    para=0;
    char *header_para=(char*)"HTTP/1.1 200 OK\r\nContent-Length: %d\r\n\r\n%s";
    char* bodyInfo= (char *)"<table border=\"1\">"
                    "<tr>"
                    "<th>Heading</th>"
                    "<th>Another Heading</th>"
                    "</tr>"
                    "<tr>"
                    "<td>row 1, cell 1</td>"
                    "<td>row 1, cell 2</td>"
                    "</tr>"
                    "<tr>"
                    "<td>row 2, cell 1</td>"
                    "<td>row 2, cell 2</td>"
                    "</tr>"
                    "</table>";
    int bodyLen=strlen(bodyInfo);
    int msglen=strlen(header_para)+strlen(bodyInfo)+1024;
    char * msgdata=(char*)malloc(msglen);
    int datalen=0;
    char recvData[2048]={0};
    int recvLen=0;
    int pos=0;
    int ret=0;
    if(!msgdata)
    {
        SYS_LOG(ZLOGWARNING,"alloc mem fail\n");
        goto END;
    }
    while(1)
    {
        ret=client.recv(recvData+pos,2048-pos,0);
        if(ret<=0)
        {
            SYS_LOG(ZLOGWARNING,"client.recv err %d\n",ret);
            goto END;
            
        }
        recvLen+=ret;
        if(recvLen>=2048)
        {
            SYS_LOG(ZLOGWARNING,"recvLen>=2048 %d\n",recvLen);
            goto END;
        }
        char * p=strstr(recvData,"\r\n\r\n");
        if(p!=0)
        {
            SYS_LOG(ZLOGWARNING,"strstr success %d\n",p);
            break;
        }
        else
        {
            SYS_LOG(ZLOGWARNING,"strstr fail %s\n",recvData);
        }
    }
    datalen=sprintf(msgdata,header_para,bodyLen,bodyInfo);
    client.send(msgdata,datalen,0);
END:
    if(msgdata)
        free(msgdata);
    msgdata=0;
	return 0;
    
}
void* createServer(void* a)
{
    tcpServer ts;
    int ret=ts.init();
    if(ret<0)
    {

        SYS_LOG(ZLOGINFO,"server init fail\n");
        return 0;
    }
    ret=ts.bind("0.0.0.0","8181");
    if(ret!=0)
    {

        SYS_LOG(ZLOGINFO,"server bind fail ret=%d\n",ret);
        return 0;
    }
    SYS_LOG(ZLOGINFO,"server bind success\n");
    ret=ts.listen(10);;
    if(ret!=0)
    {

        SYS_LOG(ZLOGINFO,"server listen fail ret=%d\n",ret);
        return 0;
    }
    zlibNetAddr  client;
    while(1)
    {
        int *clientSock=(int*)malloc(sizeof(int));
        *clientSock=ts.accept(&client);
        if(*clientSock<=0)
        {

            SYS_LOG(ZLOGINFO,"server listen fail ret=%d\n",*clientSock);
            return 0;
        }
        SYS_LOG(ZLOGINFO,"new client connect in(%s:%d)\n",inet_ntoa(*(in_addr*)&client.ip),ntohs(client.port));
        createthread(responseClient,clientSock);
        usleep(1000);
    }
}
int InitLog()
{
    
	znlog *zl=znlog::getInstance();
    zl->set_level(ZLOGINFO,ZLOGINFO);
    zl->set_log_file("test.log");
	return  0;
}
int TestMsg()
{
    Message msg;
    char buf[1024];
    sprintf(buf,"1231231123");
    int ret=0;
    ret=msg.setData(buf,strlen(buf));
    if(ret!=Message::SUCCESS)
    {
        SYS_LOG(ZLOGINFO,"message err %d\n",ret);
        return 0;
    }
    ret=msg.setType(1000);
    
    if(ret!=Message::SUCCESS)
    {
        SYS_LOG(ZLOGINFO,"message err %d\n",ret);
        return 0;
    }
    ret=msg.setMagic("IRST");
    if(ret!=Message::SUCCESS)
    {
        SYS_LOG(ZLOGINFO,"message err %d\n",ret);
        return 0;
    }
    char buf2[1024];
    ret=msg.setData2Buffer(buf2,1024);
    if(ret<Message::SUCCESS)
    {
        SYS_LOG(ZLOGINFO,"message err %d\n",ret);
        return 0;
    }
    int len=ret;
    msg.setMagic("1234");
    msg.setType(111);
    msg.setData("zc",strlen("zc"));
    msg.setData2Buffer(buf2+len,1024);
    if(ret<Message::SUCCESS)
    {
        SYS_LOG(ZLOGINFO,"message err %d\n",ret);
        return 0;
    }

    Message msg2;
    ret=msg2.getDataFromBuffer(buf2,ret);
    if(ret<Message::SUCCESS)
    {
        SYS_LOG(ZLOGINFO,"message err %d\n",ret);
        return 0;
    }
    SYS_LOG(ZLOGINFO,"read Msg %s %d %s\n",msg2.getMagic().c_str(),msg2.getType(),(char*)msg2.getData());
    len=ret;
    
    Message msg3;
    ret=msg3.getDataFromBuffer(buf2+len,1024);
     if(ret<Message::SUCCESS)
    {
        SYS_LOG(ZLOGINFO,"message err %d\n",ret);
        return 0;
    }
    SYS_LOG(ZLOGINFO,"read Msg %s %d %s\n",msg3.getMagic().c_str(),msg3.getType(),(char*)msg3.getData());
    
    MessageReader mr;
    mr.getDataFromBuffer(buf2,msg2.getSize()+msg3.getSize());
    SYS_LOG(ZLOGINFO,"MessageReader read Msg num %d\n",mr.getSize());;
	return 0;
}
int main(int argc ,char * argv[])
{
    lib_init();
    InitLog();
    TestMsg();
    // createthread(createServer,0);
    // usleep(1000*100);
    // while(1)
    // {
    //     printf(".");
    //     usleep(1000*100);
    //     fflush(stdout);
    // }
#ifdef WIN32
	getchar();
#endif
	return 0;
}