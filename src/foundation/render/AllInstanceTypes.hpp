#pragma once
#include "flywheel/api/instance/InstanceType.hpp"
#include "content/kinetics/base/RotatingInstance.hpp"

class AllInstanceTypes {
public:
    static std::shared_ptr<InstanceType<RotatingInstance>> ROTATING;
};