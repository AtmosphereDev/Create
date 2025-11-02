#pragma once
#include "flywheel/lib/instance/ColoredLitInstance.hpp"

class ColoredLitOverlayInstance : public ColoredLitInstance {
public:
    ColoredLitOverlayInstance(std::shared_ptr<InstanceTypeBase> type, std::shared_ptr<InstanceHandle> handle) 
        : ColoredLitInstance(type, handle) {}
};