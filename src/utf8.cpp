#include "utf8.h"

size_t utf::next_char(std::string_view &utf8, codepoint &c)
{
    if (utf8.size() == 0)
    {
        return 0;
    }

    // 1 byte
    if ((utf8[0] & 0b1000'0000) == 0b0000'0000)
    {
        c = utf8[0];
        utf8 = utf8.substr(1);
        return 1;
    }
    // 2 bytes
    else if ((utf8[0] & 0b1110'0000) == 0b1100'0000)
    {
        if (utf8.size() < 2) return 0;
        c = ((utf8[0] & 0b0001'1111) << 6) |
            ((utf8[1] & 0b0011'1111) << 0);
        utf8 = utf8.substr(2);
        return 2;
    }
    // 3 bytes
    else if ((utf8[0] & 0b1111'0000) == 0b1110'0000)
    {
        if (utf8.size() < 3) return 0;
        c = ((utf8[0] & 0b0000'1111) << 12) |
            ((utf8[1] & 0b0011'1111) << 6) |
            ((utf8[2] & 0b0011'1111) << 0);
        utf8 = utf8.substr(3);
        return 3;
    }
    // 4 bytes
    else if ((utf8[0] & 0b1111'1000) == 0b1111'0000)
    {
        if (utf8.size() < 4) return 0;
        c = ((utf8[0] & 0b0000'0111) << 18) |
            ((utf8[1] & 0b0011'1111) << 12) |
            ((utf8[2] & 0b0011'1111) << 6) |
            ((utf8[3] & 0b0011'1111) << 0);
        utf8 = utf8.substr(4);
        return 4;
    }
    else
    {
        // Invalid UTF-8
        return 0;
    }
}
