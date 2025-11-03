#pragma once
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include <mc/src/common/world/Facing.hpp>
#include "content/kinetics/base/KineticBlock.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include <mc/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>
#include "content/kinetics/base/ShaftVisual.hpp"
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"

class RotatedPillarKineticBlock : public KineticBlock {
public:
    RotatedPillarKineticBlock(const std::string& name, short id, const Material& material)
        : KineticBlock(name, id, material) {}

    const Block& getPlacementBlock(const Actor& unk0, const BlockPos& unk1, FacingID face, const Vec3& unk3, int unk4) const override {
        const Block* renderBlock = &getRenderBlock();

        Facing::Axis axis = Facing::getAxis(face);

        renderBlock = renderBlock->setState<Facing::Axis>(VanillaStates::PillarAxis, axis);

        return *renderBlock;
    }
};