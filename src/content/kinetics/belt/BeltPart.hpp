#pragma once
#include <cstdint>

class BeltPart {
public:
    enum Type : uint8_t {
        START,
        MIDDLE,
        END,
        PULLEY,
        MAX_VALUE
    };
};