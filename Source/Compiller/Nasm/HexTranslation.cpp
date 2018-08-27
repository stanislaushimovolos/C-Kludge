//
// Created by stanissslaf on 4/30/18.
//

#include <cstring>

const char HexNumStr[] = "0123456789abcdef";

bool convertHexToStr(double num, char *dst)
{
    if (dst == nullptr)
        return false;

    unsigned char buf[8] = {};
    memcpy(buf, &num, sizeof(double));

    for (int i = 0; i < 8; i++) {
        *(dst + 1) = HexNumStr[buf[7 - i] % 16];
        buf[7 - i] /= 16;

        *dst = HexNumStr[buf[7 - i]];
        dst += 2;
    }

    return true;

}