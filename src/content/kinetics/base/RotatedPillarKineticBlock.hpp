#pragma once
#include "content/kinetics/base/KineticBlock.hpp"
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include <mc/src/common/world/Facing.hpp>

class RotatedPillarKineticBlock : public BlockLegacy {
public:
    RotatedPillarKineticBlock(const std::string& name, short id, const Material& material)
        : BlockLegacy(name, id, material) {}

    const Block& getPlacementBlock(const Actor& unk0, const BlockPos& unk1, FacingID unk2, const Vec3& unk3, int unk4) const override {
        const Block* renderBlock = &getRenderBlock();

        Facing::Axis axis = renderBlock->getState<Facing::Axis>(VanillaStates::PillarAxis);

        Log::Info("RotatedPillarKineticBlock::getPlacementBlock called with axis: {}", (int)axis);

        renderBlock = renderBlock->setState<Facing::Axis>(VanillaStates::PillarAxis, Facing::Axis::Y);

        axis = renderBlock->getState<Facing::Axis>(VanillaStates::PillarAxis);

        Log::Info("RotatedPillarKineticBlock::getPlacementBlock after set with axis: {}", (int)axis);

        return *renderBlock;
    }
};