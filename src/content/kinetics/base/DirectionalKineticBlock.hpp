#pragma once
#include "content/kinetics/base/KineticBlock.hpp"

class DirectionalKineticBlock : public KineticBlock {
public:
    DirectionalKineticBlock(const std::string& name, short id, const Material& material)
        : KineticBlock(name, id, material) {}
};