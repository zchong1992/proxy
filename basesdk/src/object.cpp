
#include "object.h"

using namespace std;
namespace baseservice
{


    object* object::myself=0;;
    object * objmanager::m_obj=0;
    objmanager * objmanager::m_objmanager=0;
    int objmanager::enable=0;

    unsigned int objmanager::ip=0;
    unsigned short objmanager::port=0;

    string int2str(int value)
    {

        char buf[100];
        sprintf(buf,"0:%d",value);
        string ret=buf;
        return buf;
    }



    sockoop::sockoop()
    {

        type=0;
         is_writing=0;
        is_initiative=0;
    }
    int sockoop::check_is_initiative()
    {

         return is_initiative;
    }
    void sockoop::set_is_initiative(int mbool)
    {
         if(mbool<2&&mbool>=0)
              is_initiative=mbool;
    }
    void ostring::setvalue(string strs)
     {
          
               setting();
               str=strs;
             set();
     }
     int ostring::sendData(os_socket sock)
     {
          int len=str.length();
        int ret=0;
          char buf[1024];
          if(len>1000)
               return 0;
          strcpy(buf+10,str.c_str());
          len=strlen(buf+10);
          if(!is_setting())
          {
               memcpy(buf,"ZKZN",4);
               *( short*)(buf+4)=htons((short)type);
               *( int*)(buf+6)=htonl((int)len);
               ret=send(sock,buf,10,0);
               //SYS_LOG(INFO,"send %d\tsize %d %s\n",type,len,str.c_str());
               ret=send(sock,buf+10,len,0);
            if(ret==SOCKET_ERROR)
                return -1;
            return 1;
          }
          return 0;
     }
     void ostring::Init()
    {
        Initbase();
        str="";
    }
    
     void oint::setvlaue(int m)
     {
          setting();
          m_vlaue=m;
          set();
     }
     int oint::sendData(os_socket sock)
     {
          unsigned char buf[100]; 
             int ret=0;
          if(!is_setting())
          {
               memcpy(buf,"ZKZN",4);
               *( short*)(buf+4)=htons((short)type);
               *( int*)(buf+6)=htonl((int)4);
               *( int*)(buf+10)=htonl((int)m_vlaue);
               //SYS_LOG(INFO,"send %d\tsize %d value %d\n",type,4,ntohl(*(int*)(buf+10)));
               ret=send(sock,(char*)buf,14,0);
            if(ret==SOCKET_ERROR)
                return -1;
            return 1;
          }
          return 0;
     }
     void oint::Init()
    {
        Initbase();
        m_vlaue=0;
    }
    void oimg::setvlaue(unsigned char * src,int len)
     {
          if(is_setting()==0)
          {
             //cout<<"enter setvlaue"<<endl;
               if(len>buflength)
               {
                    if(m_p!=0)
                    {
                         free(m_p);
                    }
                    m_p=malloc(len);
                    if(m_p==0)
                    {
                         set();
                         length=0;
                     if(m_p!=0)
                         free(m_p);
                     m_p=0;
                         return;
                    }
                    buflength=len;
               }
             length=len;
               memcpy(m_p,src,length);
               setting();
             //cout<<"leave setvlaue"<<endl;
        }
         
     }
     int oimg::sendData(os_socket sock)
     {
          char buf[100];
        int ret=0;
          if(is_setting()==1&&m_p!=0)
          {
             //cout<<"enter sendData"<<endl;
               memcpy(buf,"ZKZN",4);
               *( short*)(buf+4)=htons((short)type);
               *( int*)(buf+6)=htonl((int)length);
               ret=send(sock,buf,10,0);
               if(ret==SOCKET_ERROR)
                    ret=-1;
            else
            {
                    ret= 1;
            }
               //SYS_LOG(INFO,"send %d\tsize %d\n",type,length);
               ret=send(sock,(char*)m_p,length,0);
               if(ret==SOCKET_ERROR)
                    ret=-1;
            else
            {
                    ret= 1;
            }
               set();
          }
        else
        {
               ret= 0;
        }
        //cout<<"leave sendData"<<endl;
        return ret;
     }
     void oimg::Init()
     {
          m_p=0;
          length=0;
        buflength=0;
        m_is_sending=0;
          Initbase();
     }
    int oimg::is_sending()
    {
        return m_is_sending;
    }
    void oimg::sended()
    {
        m_is_sending=0;
    };
    void oimg::sending()
    {
        m_is_sending=1;
    }
    int objmanager::staticlistenthread()
    {
        set_thread_title("staticlistenthread");
        os_socket sockSrv = socket( AF_INET , SOCK_STREAM , 0 );
        //服务器地址
        SOCKADDR_IN addrSrv;
        int len =sizeof(int),flag=1;

        addrSrv.sin_addr.s_addr=ip;
        addrSrv.sin_port=port;
        addrSrv.sin_family=AF_INET;

        //将socket与地址绑定在一起
        if( setsockopt(sockSrv, SOL_SOCKET, SO_REUSEADDR, &flag, len) == -1)  
        {  
            SYS_LOG(INFO,"setsockopt fail");  
            exit(0);  
        }  
        if(bind( sockSrv ,(SOCKADDR*)&addrSrv , sizeof(SOCKADDR) ) !=0)
        {
            SYS_LOG(INFO,"bind fail\n",ntohs(port));
            closesocket(sockSrv);
            exit(0);
        }
        //开始监听客户端请求,最大连接数为5
        listen( sockSrv , 1 );
        //用于存放客户端地址
        SOCKADDR_IN addrClient;
        len = sizeof( SOCKADDR_IN );
        //不断接收客户端发送的请求
        os_socket* sockConn;
    
        while(1)
        {
            sockConn=new os_socket;
            *sockConn = accept( sockSrv , (SOCKADDR *)&addrClient , (socklen_t*)&len );
            createthread((createdthread)dealclientthread,(void*)sockConn); // 创建线程
        }
    }
    void *objmanager::dealclientthread(void *mPara)
    {
    
        SYS_LOG(INFO,"socketThread connecting.....\n");
        os_socket * m_socket=(os_socket*)mPara;
        set_thread_title("dealclient%d",*m_socket);
        regres_t type[1024];
        memset(type,0,sizeof(regres_t)*1024);
        object *obj=object::Getobject();
        int length=0;
        int ret=reg(*m_socket,type,length);
        if(ret==-1)
        {
            SYS_LOG(INFO,"socketThread error reg!\n");
            goto MYERROR2;
        }
        else if(ret==1)
        {
            while(1)
            {
                ret=obj->sendobj(*m_socket,type,length);
                if(ret!=0)
                {
                    //Sleep(100);
                    goto MYERROR2;
                }
                    
                    Sleep(50);
            }
        }
        while(1)
        {
            ret=obj->sendobj(*m_socket,type,length);
            if(ret==-1)
                goto MYERROR2;
            Sleep(1);
        }
MYERROR2:
        SYS_LOG(INFO,"%d thread exit\n",*m_socket);
        closesocket(*m_socket);
        free(m_socket);
        m_socket=0;
        return 0; 
    }
    objmanager * objmanager::Getobjmanager()
    {
        if(m_objmanager==0)
        {
            m_objmanager = new objmanager();
        }
        return m_objmanager;
    };
    
    int objmanager::reg(os_socket sock,regres_t type[],int &outlength)
    {
        unsigned char buf[1024];
        int recvlen=0;
        int length=10;
        int ret=0;
        int itemlen=0;
        int i;
        int regtype=-1;
          int heartbeat=0;
        outlength=0;
        while(recvlen<length)
        {
            ret=recv(sock,(char*)buf+recvlen,1024-recvlen,0);
            if(ret<1)
            {
                SYS_LOG(INFO,"recv error 1!\n");
                return -1;
            }
            recvlen+=ret;
            if(recvlen<10)
                continue;
            break;
        }
        regtype=ntohs(*(unsigned short*)(buf+4));
        itemlen=ntohl(*(int *)(buf+6));
        if(itemlen>1024||itemlen==0)
        {
            SYS_LOG(INFO,"reg error length\n");
            return -1;
        }
        if(0==regtype)
        {
                heartbeat=1;
        }
        else if(1==regtype)
        {
            SYS_LOG(INFO,"once quester!\n");
        }
        else 
        {
            SYS_LOG(INFO,"type error!\n");
            return -1;
        }
        length+=itemlen;
        while(recvlen<length)
        {
            ret=recv(sock,(char*)buf+recvlen,1024-recvlen,0);
            if(ret<1)
            {
                SYS_LOG(INFO,"recv error 2!\n");
                return -1;
            }
            recvlen+=ret;
        }
        SYS_LOG(INFO,"client need :\t");
        for(i=0;i<length-10;i+=2)
        {
            type[outlength].type=ntohs(*(short*)(buf+10+i));
            SYS_LOG_NO_TIME(INFO,"%d\t",type[outlength].type);
            outlength++;
        }
        SYS_LOG_NO_TIME(INFO,"\n");
          if(heartbeat)
          {
               type[outlength].type=8888;
            outlength++;
          }
        return regtype;
    }
     static void heart_beat(void* Para)
     {
          object     *obj=object::Getobject();
          while(1)
          {
               obj->heart_beat.setvalue("");
               Sleep(5000);
          }
          
     }
    void objmanager::start()
    {
         if(port!=0)
         {
              createthread((createdthread)staticlistenthread,0);
              createthread((createdthread)heart_beat,0);
              SYS_LOG(INFO,"start server thread\n");
              return;
         }
         SYS_LOG(INFO,"set ip and port before start\n");
    }
    void objmanager::setserveraddr(char *tip,unsigned short tport)
    {
         if(tip!=0)
              ip=inet_addr(tip);
         port=htons(tport);
    }
    int object::sendobj(os_socket sock,regres_t type[],int length)
    {
        int ret=0;
        int num=0;
         for (int i =0; i < length; i++)
         {
              sockoop* oo=m_map[type[i].type];
              if(oo!=0&&oo->last_set_time>type[i].last_send_time)
              {
                  ret=oo->sendData(sock);
                  if(ret==-1)
                        return -1;
                type[i].last_send_time=oo->last_set_time;
                num+=ret;
              }
               else
               {
               }
               
         }
         return num;
    }
     void object::addItem(sockoop & item,int index)
     {
          item.Init();
          item.type=index;
          m_map.insert(pair<int,sockoop*>(index,&item));
     }
    object::object()
    {
        m_map.clear();
        addItem(facename,2);
        addItem(irisname,4);
        addItem(score,6);
        addItem(log,10);
        addItem(hlefteye,11);
        addItem(isfacedetect,12);
        addItem(hrighteye,13);
        addItem(iseyestable,14);
        addItem(facefps,16);
        addItem(iris0fps,18);
        addItem(iris1fps,20);
        addItem(id,22);
        addItem(facex,24);
        addItem(facey,26);
        addItem(iris0x,28);
        addItem(iris0y,30);
        addItem(iris1x,32);
        addItem(iris1y,34);
        addItem(facerighteyex,36);
        addItem(facerighteyey,38);
        addItem(facelefteyex,40);
        addItem(facelefteyey,42);
        addItem(irisrighteyex,44);
        addItem(irisrighteyey,46);
        addItem(irislefteyex,48);
        addItem(irislefteyey,50);
        addItem(focurscorex,52);
        addItem(focurscorey,54);

          addItem(irisreg,100);
        addItem(iris0img,101);
        addItem(faceimg,103);
          addItem(facereg,104);
          addItem(distance,106);
        addItem(lefteyeimg,107);
          addItem( tofreg,108);
          addItem(righteyeimg,109);
          addItem(tofimgcat,105);
          addItem(tofrawcat,112);
          addItem(lampstatus,114);
          addItem(brightness,116);
          addItem(fpgadeadloop,118);
          addItem(fpgaddrhurt,120);
          addItem(fpgaspeed,122);
          addItem(fpgamemfuntion,124);
          addItem(fpgamemspeed,126);
          addItem(armstatus,128);
          addItem(armspeed,130);
          addItem(irisimgtransportspeed,204);
        addItem(irisimgnoise,206);
          addItem(faceimgtransportspeed,208);
          addItem(faceimgtransportnoise,210);
          addItem(tofimgtransportspeed,212);
        addItem(tofimgnoise,214);
          addItem(tofimgdealspeed,216);
          addItem(regopt,228);
          addItem(regoptquality,230);
          addItem(deletequalitimg,232);
          addItem(optstartorstop,234);
          addItem(vioceswitch,236);
          addItem(deleteresult,238);
          addItem(logstatisticstatus,240);
          addItem(loglook,242);
          addItem(logseek,244);
          addItem(logseeksave,246);
          addItem(facedeteceresult,260);
          addItem(facedetecspeed,262);
        addItem(imgpancalerr,264);
        addItem(imgtofcalerr,266);
          addItem(locationresult,268);
          addItem(localtionspeed,270);
        addItem(locationerr,272);
        addItem(tofacc,274);
          addItem(tofspeed,276);
          addItem(predealresult,278);
        addItem(predealspeed,280);
          addItem(qualityresult,282);
          addItem(qualityspeed,284);
          addItem(irissdkspeed,286);
          addItem(irissuccess,288);
        addItem(log7d,300);
        addItem(loadlog,302);
          addItem(rgbImgFaceRect,304);

          
          

          addItem(sirVersion,306);
          addItem(algoVersion,308);
          addItem(hardVersion,310);
          addItem(hardLibVersion,312);
          

        
                    
          addItem(enrolleyepos,7000);
          addItem(enrollGraphGrayLevelValue,7001);
          addItem(enrollGraphGrayAvarValue,7002);
          addItem(enrollGraphFocusValue,7003);
          addItem(enrollGraphLocationValue,7004);
          addItem(enrollGraphPercentVisibleValue,7005);
          addItem(enrollGraphReflactionRationROIValue,7006);
          
          addItem(heart_beat,8888);
      
        
        
       
        //
    }

}

