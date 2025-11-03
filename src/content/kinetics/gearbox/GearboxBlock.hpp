#pragma once
#include "content/kinetics/base/RotatedPillarKineticBlock.hpp"

class GearboxBlock : public RotatedPillarKineticBlock {
public:
    GearboxBlock(const std::string& name, int id, Material& material)
        : RotatedPillarKineticBlock(name, id, material) {}

    virtual std::shared_ptr<BlockActor> newBlockEntity(const BlockPos& pos, const Block& block) const override {
		return std::make_shared<KineticBlockEntity>(JavaBlockEntity::TYPE, pos, "");
	}

    virtual Facing::Axis getRotationAxis(const Block& state) const override {
        return state.getState<Facing::Axis>(VanillaStates::PillarAxis);
    }
};