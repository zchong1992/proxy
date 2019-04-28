
 /*!
* \file config.h
* \brief 配置文件读写
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/
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
/**
* \brief znconfig 配置文件读写类
*/
    class znconfig
    {
    public:
/**
* \brief loadfile 从文件中读取配置文件
* \params filename 文件路径
* \return  <0 读取失败, >=0 读取成功
*/
        int loadfile(const char * filename);

/**
* \brief  insertData 插入数据
* \params key 数据名
* \params value 数据值
* \return  <0 读取失败, >=0 读取成功
*/
        int insertData(const char *key,const char*value);

/**
* \brief  print 打印所有数据内容
* \return  无返回值
*/
        int print();
    

/**
* \brief  save 保存数据到文件中
* \return  <0 保存失败  >=0 保存成功
*/
        int save(const char *path);
    
/**
* \brief  add 插入数据
* \params key 数据名
* \params value 数据值
* \return  <0 插入失败, >=0 插入成功
*/
        int add(const char *key,const char* vlaue);
    
/**
* \brief  del 删除数据
* \params key 数据名
* \return  <0 删除失败, >=0 删除成功
*/
        int del(const char *key);
    
   
/**
* \brief  Getvalue 获得数据
* \params key 数据名
* \return  <0  获得失败, >=0  获得成功
*/
        const char * Getvalue(const char *key);
        znconfig();
        ~znconfig();
    private:
        zkv * m_zkv;
    };
    

}
#endif 

