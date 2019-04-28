
 /*!
* \file pointer.h
* \brief 简单智能指针实现,未详细测试,后期替换为stl内智能指针
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/

#ifndef __POINTER_H__
#define __POINTER_H__

#include "global.h"
///
// The Pointer class work for temporary stack varible ,it will delete self when out of the range of action scope;
//
//
///
namespace baseservice
{

/**
* \brief 智能指针类,自动析构删除buffer
*/     template<typename T> class Pointer
     {
          public :
               T * _object;
               Pointer():_object(0){};
               Pointer(T *_obj):_object(_obj){}
               ~Pointer()
               {     
                    Release();
               }
            void Release()
            {
                    if(_object)
                         delete _object;
                    _object=0;
            }
               template<class Y> Pointer& operator=(Y* o) 
               {
                    return *this;
               }

               T& operator*() 
               {
                    return *_object;
               }
               T* operator->() const
               {

                    return _object;
               }
               operator T*() const 
               {

                    return _object;
               }
               bool operator==(const T* that) 
               {
                    return _object == that;
               }
               bool operator==(const Pointer &other) const
             {

                    return _object == other._object || *_object == *(other._object);
               }
               template<class Y> bool operator==(const Pointer<Y> &other) const 
             {
                    return _object == other._object || *_object == *(other._object);
               }

               bool operator!=(const T* that) 
               {
                    return !(*this == that);
               }
private:
               Pointer& operator=(const Pointer &other) 
               {
                    return *this;
               }
               template<class Y> Pointer& operator=(const Pointer<Y> &other) 
               {
                    return *this;
               }
               Pointer& operator=(T* o)
               {
                    return *this;
               }
             
     };
}

#endif

