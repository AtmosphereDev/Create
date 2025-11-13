#pragma once
#include "content/kinetics/base/KineticBlock.hpp"
#include <mc/src/common/world/Facing.hpp>
#include <mc/src/common/world/level/block/BlockState.hpp>

class HorizontalKineticBlock : public KineticBlock {
public:
    static CustomBlockState<FacingID>& HORIZONTAL_FACING() {
        static CustomBlockState<FacingID> instance("horizontal_facing", (uint64_t)FacingID::MAX);
        return instance;
    }

    HorizontalKineticBlock(const std::string& name, short id, const Material& material)
        : KineticBlock(name, id, material) {}
};