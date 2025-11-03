#pragma once
#include "content/kinetics/base/IRotate.hpp"

class KineticBlock : public IRotate {
public:
    KineticBlock(const std::string& name, short id, const Material& material)
        : IRotate(name, id, material) {}
};