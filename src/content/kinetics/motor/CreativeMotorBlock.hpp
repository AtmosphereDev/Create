#pragma once
#include "content/kinetics/base/DirectionalKineticBlock.hpp"
#include "content/kinetics/motor/CreativeMotorBlockEntity.hpp"

class CreativeMotorBlock : public DirectionalKineticBlock {
public:
    CreativeMotorBlock(const std::string& name, short id, const Material& material)
        : DirectionalKineticBlock(name, id, material) {}

    virtual std::shared_ptr<BlockActor> newBlockEntity(const BlockPos& pos, const Block& block) const override {
		return std::make_shared<CreativeMotorBlockEntity>(pos, "CreativeMotorBlock");
	}

    virtual Facing::Axis getRotationAxis(const Block& state) const override {
        auto dir = state.getState<Facing::Name>(VanillaStates::FacingDirection);
        return Facing::getAxis(dir);
    }

    virtual bool hasShaftTowards(const Dimension& world, const BlockPos& pos, const Block& state, FacingID face) const override {
        return face == state.getState<FacingID>(VanillaStates::FacingDirection); 
    }

    const Block& getPlacementBlock(const Actor& unk0, const BlockPos& unk1, FacingID face, const Vec3& unk3, int unk4) const override {
        const Block* renderBlock = &getRenderBlock();
        renderBlock = renderBlock->setState<FacingID>(VanillaStates::FacingDirection, face);
        return *renderBlock;
    }
};