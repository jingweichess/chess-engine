#pragma once

#include "hash.h"

struct DefaultBookPosition {
    Hash hashValue;
};

static DefaultBookPosition& operator += (DefaultBookPosition& bp1, DefaultBookPosition bp2)
{
    return bp1;
}
