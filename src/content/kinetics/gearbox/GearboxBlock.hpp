#pragma once
#include "content/kinetics/base/RotatedPillarKineticBlock.hpp"

class GearboxBlock : public RotatedPillarKineticBlock {
public:
    GearboxBlock(const std::string& name, int id, Material& material)
        : RotatedPillarKineticBlock(name, id, material) {}

    const Block& getPlacementBlock(const Actor& unk0, const BlockPos& unk1, FacingID face, const Vec3& unk3, int unk4) const override {
        const Block* renderBlock = &getRenderBlock();
        renderBlock = renderBlock->setState<Facing::Axis>(VanillaStates::PillarAxis, Facing::Axis::Y);
        return *renderBlock;
    }

    virtual std::shared_ptr<BlockActor> newBlockEntity(const BlockPos& pos, const Block& block) const override {
		return std::make_shared<KineticBlockEntity>(JavaBlockEntity::TYPE, pos, "");
	}

    virtual Facing::Axis getRotationAxis(const Block& state) const override {
        return state.getState<Facing::Axis>(VanillaStates::PillarAxis);
    }

    virtual bool hasShaftTowards(const Dimension& world, const BlockPos& pos, const Block& state, FacingID face) const override {
        return Facing::getAxis(face) != state.getState<Facing::Axis>(VanillaStates::PillarAxis); 
    }
};