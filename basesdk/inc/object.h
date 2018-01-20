#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <string>
#include <map>
#include <stdio.h>
#include <iostream>
#include <string>

#include "global.h"
#include "log.h"

namespace baseservice
{

std::string int2str(int value);
typedef struct regstruct_s
{
     UINT1 has_err;
    UINT1 res_1;
     UINT2 type;
    UINT4 res_4;
    UINT8 last_send_time;
}regres_t;

struct sockoop{
    int check_is_initiative();
    void set_is_initiative(int mbool);
     int is_setting() {return is_writing;}
    void setting(){is_writing=1;last_set_time=GetCurTimems();}
    void set(){is_writing=0;}
     void Initbase(){is_initiative=1;is_writing=0;type=0;last_set_time=0;};
     void set_type(int t){type=t;}
     virtual int sendData(os_socket sock)=0;
    virtual void Init()=0;
     unsigned short type;
    UINT8 last_set_time;
    sockoop();
private:
     int is_writing;
    int is_initiative;
};

struct ostring:sockoop
{
     char buf[100];
     void setvalue(std::string strs);
     int sendData(os_socket sock);
     void Init();
private:
     std::string str; 

};
struct oimg:sockoop
{
     void setvlaue(unsigned char * src,int len);
     int sendData(os_socket sock);
     void Init();
    int is_sending();
    void sended();
    void sending();
private:
    int m_is_sending;
     int length;
     int buflength;
     void *m_p;
};
struct oint:sockoop
{
     int m_vlaue;
     void setvlaue(int m);
     int sendData(os_socket sock);
     void Init();
};
class object{
public :
     static object* Getobject()
     {
          if(myself==0)
          {
               myself =new object();
               return myself;
          }
          return myself;
     };
     int sendobj(os_socket sock,regres_t type[],int length);
private :
     object();
public:
     std::map<int,sockoop*> m_map;

    void addItem(sockoop & item,int index);
     ostring heart_beat;
     ostring facename;
     ostring irisname;
     ostring score;
     ostring distance;
     ostring log;
    
     ostring sirVersion;
     ostring algoVersion;
     ostring hardVersion;
     ostring hardLibVersion;
     oimg faceimg;
     oimg iris0img;
     //oimg iris1img;
     oimg lefteyeimg;
     oimg righteyeimg;
     oimg hlefteye;
     oimg hrighteye;
    
     ostring isfacedetect;
     ostring iseyestable;
     ostring facefps;
     ostring iris0fps;
     ostring iris1fps;
     ostring id;
     ostring rgbImgFaceRect;
    
     ostring facex;
     ostring iris0x;
     ostring iris1x;
     ostring facerighteyex;
     ostring facelefteyex;
     ostring irisrighteyex;
     ostring irislefteyex;
     ostring focurscorex;
    ostring facey;
     ostring iris0y;
     ostring iris1y;
     ostring facerighteyey;
     ostring facelefteyey;
     ostring irisrighteyey;
     ostring irislefteyey;
     ostring focurscorey;


    //

     
    ostring irisreg;
    //oimg    irisimgcat;
    ostring facereg;
    //oimg    faceimgcat;
    ostring tofreg;
    oimg    tofimgcat;
    oimg    tofrawcat;
    ostring lampstatus;
    ostring brightness;
    ostring fpgadeadloop;
    ostring fpgaddrhurt;
    ostring fpgaspeed;
    ostring fpgamemfuntion;
    ostring fpgamemspeed;
    ostring armstatus;
    ostring armspeed;
    ostring irisimgtransportspeed;
    ostring faceimgtransportspeed;
    ostring faceimgtransportnoise;
    ostring tofimgtransportspeed;
    ostring tofimgdealspeed;
    ostring regopt;
    ostring regoptquality;
    ostring deletequalitimg;
    ostring optstartorstop;
    ostring vioceswitch;
    ostring deleteresult;
    ostring logstatisticstatus;
    ostring loglook;
    ostring logseek;
    ostring logseeksave;
    ostring facedeteceresult;
    ostring facedetecspeed;
    ostring locationresult;
    ostring localtionspeed;
    ostring tofspeed;
    ostring predealresult;
    ostring qualityresult;
    ostring qualityspeed;
    ostring irissdkspeed;
    ostring irissuccess;
    //


    ////

    ostring irisimgnoise    ;
    ostring tofimgnoise ;
    ostring imgpancalerr    ;
    ostring imgtofcalerr    ;
    ostring locationerr ;
    ostring tofacc  ;
    ostring predealspeed    ;
    ostring log7d   ;
    ostring loadlog ;

    /////
          
     ostring enrolleyepos;
     ostring enrollGraphGrayLevelValue;
     ostring enrollGraphGrayAvarValue;
     ostring enrollGraphFocusValue;
     ostring enrollGraphLocationValue;
     ostring enrollGraphPercentVisibleValue;
     ostring enrollGraphReflactionRationROIValue;



    
     static object* myself;
};

class objmanager{
public:
     void start();
     void setserveraddr(char *ip,unsigned short port);
     
     static int staticlistenthread();
     static void *dealclientthread(void *mPara);
     static objmanager * Getobjmanager();
private:
     
     static int reg(os_socket sock,regres_t type[],int &outlength);
     objmanager(){};
     static unsigned int ip;
     static unsigned short port;
     static object * m_obj;
     static objmanager * m_objmanager;
     static int enable;

};

}
#endif
