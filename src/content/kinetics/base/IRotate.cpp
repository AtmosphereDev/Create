#include "IRotate.hpp"
#include "infrastructure/config/AllConfigs.hpp"

IRotate::SpeedLevel IRotate::SpeedLevel::of(float speed)
{
    speed = std::abs(speed);

    if (speed >= AllConfigs::server().kinetics.fastSpeed.get())
        return FAST;
    if (speed >= AllConfigs::server().kinetics.mediumSpeed.get())
        return MEDIUM;
    if (speed >= 1)
        return SLOW;
    return NONE;
}