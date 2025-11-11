#pragma once
#include "content/kinetics/base/DirectionalShaftHalvesBlockEntity.hpp"

class GearboxBlockEntity : public DirectionalShaftHalvesBlockEntity {
public:
    GearboxBlockEntity(const BlockPos& pos, const std::string& type)
        : DirectionalShaftHalvesBlockEntity(pos, type) {}

    virtual bool isNoisy() const override {
        return false;
    }
};