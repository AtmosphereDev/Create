#pragma once
#include "content/kinetics/base/KineticBlock.hpp"

class HorizontalKineticBlock : public KineticBlock {
public:
    HorizontalKineticBlock(const std::string& name, short id, const Material& material)
        : KineticBlock(name, id, material) {}
};