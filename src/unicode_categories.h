#pragma once

#include "utf8.h"

namespace utf
{
    using unicode_classifier = bool(*)(codepoint);

    namespace classifiers
    {
        bool whitespace(codepoint c);
        bool not_whitespace(codepoint c);

        bool xid_start(codepoint c);
        bool xid_continue(codepoint c);

        bool emoji_presentation(codepoint c);
        bool emoji_modifier(codepoint c);
        bool emoji_modifier_base(codepoint c);
    }
}
