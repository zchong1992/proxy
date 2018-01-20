/*
 * FileName : AES.h
 * Get From GitHub and zc fix some error
 * eamil :zhengchong_china@163.com
 */
#ifndef __AES_H__
#define __AES_H__

#include "libbaseservice.h"
namespace baseservice
{
class AES  
{
public:
     AES(UINT1* key = NULL);
     virtual ~AES();
     void SetKey(UINT1 *key);
     UINT1* Cipher(UINT1* input, UINT1* output);
     UINT1* InvCipher(UINT1* input, UINT1* output);
     void* Cipher(void* input, void *output, int length=0);
     void* InvCipher(void* input,void *output, int length);

private:
     UINT1 Sbox[256];
     UINT1 InvSbox[256];
     UINT1 w[11][4][4];
     UINT1 FFmul_speed[256][256];

     void KeyExpansion(UINT1* key, UINT1 w[][4][4]);
     UINT1 FFmul(UINT1 a, UINT1 b);
     UINT1 FFmul_fast(UINT1 a, UINT1 b);
     void FFmul_init();

     void SubBytes(UINT1 state[][4]);
     void ShiftRows(UINT1 state[][4]);
     void MixColumns(UINT1 state[][4]);
     void AddRoundKey(UINT1 state[][4], UINT1 k[][4]);

     void InvSubBytes(UINT1 state[][4]);
     void InvShiftRows(UINT1 state[][4]);
     void InvMixColumns(UINT1 state[][4]);
};

enum AESMode_t { MODE_OFB = 1, MODE_CFB, MODE_CBC, MODE_ECB };
class AESModeOfOperation {
private:
     AES *m_aes;
     AESMode_t       m_mode;
     UINT1 m_key[16];
     UINT1 m_iv[16];
//     bool            m_firstround;
public:
     AESModeOfOperation();
     ~AESModeOfOperation();
     void set_mode(AESMode_t _mode);
     //AESMode_t get_mode();
     void set_key (UINT1 *key);
     void set_iv(UINT1 *iv);
     int  Encrypt(const UINT1 *input, int length, UINT1 *output);
     int  Decrypt(const UINT1 *input, int length, UINT1 *output);
};


}

#endif 
