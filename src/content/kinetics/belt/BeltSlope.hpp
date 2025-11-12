#pragma once

class BeltSlope {
public:
    enum Type : uint8_t {
        HORIZONTAL,
        UPWARD,
        DOWNWARD,
        VERTICAL,
        SIDEWAYS,
        MAX_VALUE
    };

    static bool isDiagonal(Type type) {
        return type == UPWARD || type == DOWNWARD;
    }
};