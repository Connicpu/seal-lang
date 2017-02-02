#pragma once

#include "utf8.h"

namespace utf
{
    using unicode_classifier = bool(*)(codepoint);

    namespace classifiers
    {
        bool xid_start(codepoint c);
        bool xid_continue(codepoint c);
        bool emoji(codepoint c);
    }
}
