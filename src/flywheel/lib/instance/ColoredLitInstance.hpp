#pragma once
#include "flywheel/lib/instance/AbstractInstance.hpp"

class ColoredLitInstance : public AbstractInstance {
public:
    ColoredLitInstance(std::shared_ptr<InstanceTypeBase> type, std::shared_ptr<InstanceHandle> handle) 
        : AbstractInstance(type, handle) {}
};