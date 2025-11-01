#pragma once
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include <mc/src/common/world/Facing.hpp>
#include "content/kinetics/base/KineticBlock.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"

class TestBlockActor : public BlockActor {
public:
    TestBlockActor(BlockActorType type, const BlockPos& pos, const std::string& id) 
        : BlockActor(type, pos, id) 
    {
        Log::Info("TestBlockActor created at position: ({}, {}, {}) with id: {}", pos.x, pos.y, pos.z, id);
    }
};

class RotatedPillarKineticBlock : public BlockLegacy {
public:
    RotatedPillarKineticBlock(const std::string& name, short id, const Material& material)
        : BlockLegacy(name, id, material) 
        {
            mBlockEntityType = JavaBlockEntity::TYPE;
        }

    virtual std::shared_ptr<BlockActor> newBlockEntity(const BlockPos& pos, const Block& block) const override {
		return std::make_shared<TestBlockActor>(JavaBlockEntity::TYPE, pos, "bosh");
	}

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