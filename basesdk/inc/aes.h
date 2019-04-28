 /*!
* \file aes.h
* \brief 该头文件主要是用于aes的加密解密部分,目前支持MODE_OFB = 1, MODE_CFB, MODE_CBC, MODE_ECB  四种的128位加解密
* \author zhengchong
* \email  zhengchong@iristar.com.cn
*/
#ifndef __AES_H__
#define __AES_H__

#include "global.h"

namespace baseservice
{
    
/**
* \brief aes加解密操作基础类,不直接调用
*/
class AES  
{
public:
     AES(ZUINT1* key = NULL);
     virtual ~AES();
     void SetKey(ZUINT1 *key);
     ZUINT1* Cipher(ZUINT1* input, ZUINT1* output);
     ZUINT1* InvCipher(ZUINT1* input, ZUINT1* output);
     void* Cipher(void* input, void *output, int length=0);
     void* InvCipher(void* input,void *output, int length);

private:
     ZUINT1 Sbox[256];
     ZUINT1 InvSbox[256];
     ZUINT1 w[11][4][4];
     ZUINT1 FFmul_speed[256][256];

     void KeyExpansion(ZUINT1* key, ZUINT1 w[][4][4]);
     ZUINT1 FFmul(ZUINT1 a, ZUINT1 b);
     ZUINT1 FFmul_fast(ZUINT1 a, ZUINT1 b);
     void FFmul_init();

     void SubBytes(ZUINT1 state[][4]);
     void ShiftRows(ZUINT1 state[][4]);
     void MixColumns(ZUINT1 state[][4]);
     void AddRoundKey(ZUINT1 state[][4], ZUINT1 k[][4]);

     void InvSubBytes(ZUINT1 state[][4]);
     void InvShiftRows(ZUINT1 state[][4]);
     void InvMixColumns(ZUINT1 state[][4]);
};
  
/**
* \brief aes加解密类型枚举
*/
enum AESMode_t { MODE_OFB = 1, MODE_CFB, MODE_CBC, MODE_ECB };

/**
* \brief aes加解密操作类
*/
class AESModeOfOperation {
private:
     AES *m_aes;
     AESMode_t       m_mode;
     ZUINT1 m_key[16];
     ZUINT1 m_iv[16];
//     bool            m_firstround;
public:
     AESModeOfOperation();
     ~AESModeOfOperation();

/**
* \brief set_mode 加密方式选择类别  MODE_OFB , MODE_CFB, MODE_CBC, MODE_ECB
* \param _mode  可选 MODE_OFB , MODE_CFB, MODE_CBC, MODE_ECB
* \return 无返回值
*/
     void set_mode(AESMode_t _mode);
     //AESMode_t get_mode();

/**
* \brief set_key 加密密钥设置 传入一个字符串作为密钥
* \param key 字符串
* \return 无返回值
*/
     void set_key (ZUINT1 *key);

/**
* \brief set_iv 设置基础向量 传入一个16字节的buff设置为启动向量,建议传入全0的buffer,长度为16字节
* \param iv 16字节的buff启动向量
* \return 无返回值
*/
     void set_iv(ZUINT1 *iv);

/**
* \brief Encrypt 加密数据,从input字段读入length长度的数据,并按16倍数向上取整写入output缓冲区内.
* \param input 待加密内容
* \param length 待加密长度
* \param output 输出缓冲区 
* \return >0 调用成功并输出转换后长度,一般为 (length+15/16)*16 ,<=0 加密失败
*/
     int  Encrypt(const ZUINT1 *input, int length, ZUINT1 *output);


/**
* \brief Decrypt 解密数据,从input字段读入length长度的数据,并按16倍数向上取整写入output缓冲区内.
* \param input 待解密内容
* \param length 待解密长度
* \param output 输出缓冲区 
* \return >0 调用成功并输出转换后长度,一般为 (length+15/16)*16 ,<=0 解密失败
*/
     int  Decrypt(const ZUINT1 *input, int length, ZUINT1 *output);
};


}

#endif 
