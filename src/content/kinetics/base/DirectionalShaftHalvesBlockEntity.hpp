#pragma once
#include "content/kinetics/base/KineticBlockEntity.hpp"

class DirectionalShaftHalvesBlockEntity : public KineticBlockEntity {
public:
    DirectionalShaftHalvesBlockEntity(const BlockPos& pos, const std::string& type)
        : KineticBlockEntity(pos, type) {}
    
    FacingID getSourceFacing() const {
        BlockPos localSource = source.value() - mPosition;
        return Facing::getNearest(localSource);
    }
};