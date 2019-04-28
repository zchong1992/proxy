#include "base64.h"
#include "global.h"
namespace baseservice
{

static unsigned char *base64table = (ZUINT1*)"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static unsigned char base64decodebtable[256] = {0};

unsigned char *byte3to4(unsigned char out[4], unsigned char pin[3], int length) {
    unsigned char in[3] = {0};
    int i = 0;
    for (i = 0; i < length; i++)
        in[i] = pin[i];
    out[0] = in[0] >> 2;
    out[1] = (((in[0] << 6) | (in[1] >> 2)) >> 2) & 0x3f;
    out[2] = (((in[1] << 4) | (in[2] >> 4)) >> 2) & 0x3f;
    out[3] = in[2] & 0x3f;

    out[0] = base64table[out[0]];
    out[1] = base64table[out[1]];
    out[2] = base64table[out[2]];
    out[3] = base64table[out[3]];
    if (length == 1) {
        out[2] = '=';
        out[3] = '=';
    } else if (length == 2) {

        out[3] = '=';
    }

    return out;
}
void inittable() {
    int length = strlen((ZINT1*)base64table);
    int i;
    for (i = 0; i < 64; i++) {
        base64decodebtable[base64table[i]] = i;
    }
    base64decodebtable['='] = 0;
}
unsigned char *byte4to3(unsigned char out[3], unsigned char pin[4]) {
    unsigned char *in = pin;
    unsigned char min[4];
    int i = 0;
    min[0] = base64decodebtable[in[0]];
    min[1] = base64decodebtable[in[1]];
    min[2] = base64decodebtable[in[2]];
    min[3] = base64decodebtable[in[3]];
    in = min;
    out[0] = ((in[0] << 2) & 0xfc) | ((in[1] >> 4) & 0x3);
    out[1] = in[1] << 4 | in[2] >> 2;
    out[2] = in[2] << 6 | in[3];

    return out;
}

int base64encode(unsigned char *out, unsigned char *in, int length) {
    int i = 0;
    unsigned char *inp, *outp;
    inp = in;
    outp = out;
    int len = 0;
    for (i = 0; i + 3 < length; i += 3) {
        byte3to4(outp, inp, 3);
        inp += 3;
        outp += 4;
        len += 4;
    }
    byte3to4(outp, inp, length - i);
    len += 4;
    return len;
}

int base64decode(unsigned char *out, unsigned char *in, int length) {
    inittable();
    int i = 0;
    unsigned char *inp, *outp;
    inp = in;
    outp = out;

    int len = 0;
    for (i = 0; i < length; i += 4) {
        byte4to3((unsigned char *)outp, (unsigned char *)inp);
        inp += 4;
        outp += 3;
        len += 3;
    }
    return len;
}
} // namespace baseservice
