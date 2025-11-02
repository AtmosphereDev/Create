#pragma once
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include <mc/src/common/world/Facing.hpp>
#include "content/kinetics/base/KineticBlock.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include <mc/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>
#include "content/kinetics/base/ShaftVisual.hpp"
#include "flywheel/api/visualization/VisualizationContext.hpp" 
#include "flywheel/api/instance/InstancerProvider.hpp"

class TestVisualizationContext : public VisualizationContext {
public:
    InstancerProvider _instancerProvider;

    virtual InstancerProvider& instancerProvider() const override {
        return const_cast<InstancerProvider&>(_instancerProvider);
    }

    virtual const BlockPos& renderOrigin() const override {
        static BlockPos origin = BlockPos::ZERO;
        return origin;
    }
};

class TestBlockActor : public KineticBlockEntity {
public:
    static TestVisualizationContext globalVisualizationContext;

    std::unique_ptr<ShaftVisual<TestBlockActor>> testVisuals;

    TestBlockActor(BlockActorType type, const BlockPos& pos, const std::string& id) 
        : KineticBlockEntity(type, pos, id), testVisuals(nullptr)
    {
        Log::Info("TestBlockActor created at position: ({}, {}, {}) with id: {}", pos.x, pos.y, pos.z, id);
        mRendererId = (BlockActorRendererId)((int)BlockActorRendererId::Count + 1);
        testVisuals = std::make_unique<ShaftVisual<TestBlockActor>>(globalVisualizationContext, this, 0.0f);
    }
};

class TestBlockActorRenderer : public BlockActorRenderer {
public:
    TestBlockActorRenderer() : BlockActorRenderer() {};

    virtual void render(BaseActorRenderContext& ctx, BlockActorRenderData& data) override {
        TestBlockActor& actor = static_cast<TestBlockActor&>(data.entity);
        // actor.testVisuals.
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