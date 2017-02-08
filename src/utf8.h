#pragma once

#include <stdint.h>
#include <string_view>

namespace utf
{
    using codepoint = uint32_t;
    size_t next_char(std::string_view &utf8, codepoint &c);
}
