#pragma once
#include "content/kinetics/base/HorizontalKineticBlock.hpp"
#include <mc/src/common/world/level/block/BlockState.hpp>
#include "content/kinetics/belt/BeltSlope.hpp"
#include "content/kinetics/belt/BeltPart.hpp"   

class BeltBlock : public HorizontalKineticBlock {
public:
    static CustomBlockState<BeltSlope::Type>& SLOPE() {
        static CustomBlockState<BeltSlope::Type> instance("slope", BeltSlope::MAX_VALUE);
        return instance;
    }

    static CustomBlockState<BeltPart::Type>& PART() {
        static CustomBlockState<BeltPart::Type> instance("part", BeltPart::MAX_VALUE);
        return instance;
    }

    BeltBlock(const std::string& name, short id, const Material& material)
        : HorizontalKineticBlock(name, id, material) {}

    Facing::Axis getRotationAxis(const Block& state) const override {
        if (state.getState<BeltSlope::Type>(SLOPE()) == BeltSlope::SIDEWAYS) {
            return Facing::Axis::Y;
        }
        return 
    }

    const Block& getPlacementBlock(const Actor& unk0, const BlockPos& unk1, FacingID face, const Vec3& unk3, int unk4) const override {
        const Block* renderBlock = &getRenderBlock();
        renderBlock = renderBlock->setState<BeltPart::Type>(SLOPE(), BeltPart::PULLEY);

        Log::Info("getState {}", (uint64_t)renderBlock->getState<BeltPart::Type>(SLOPE()));

        return *renderBlock;
    }
};