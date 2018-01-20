
#ifndef __CONFIG_H__
#define __CONFIG_H__
#include "global.h"
#include "log.h"



namespace baseservice
{
    typedef  struct znconfigkv{
        std::string key;
        std::string value;
        struct znconfigkv *next;
        znconfigkv(){key="";value="";next=0;}
    }zkv;
    class znconfig
    {
    public:
        int loadfile(char * finename);
        static znconfig* Getznconfig();
        int insertData(char *,char*);
        int print();
        void save(char *path);
        int add(char *key,char* vlaue);
        int del(char *key);
        const char * Getvalue(char *key);
    private:
        zkv * m_zkv;
        znconfig();
        ~znconfig();
        static znconfig* m_znconfig;
    };
    

}
#endif 

