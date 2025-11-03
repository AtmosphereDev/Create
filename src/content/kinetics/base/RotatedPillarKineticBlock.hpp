#pragma once
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include <mc/src/common/world/Facing.hpp>
#include "content/kinetics/base/KineticBlock.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include <mc/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>
#include "content/kinetics/base/ShaftVisual.hpp"
#include "flywheel/api/visualization/VisualizationContext.hpp" 
#include "flywheel/backend/engine/InstancerProviderImpl.hpp"
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"

class TestVisualizationContext : public VisualizationContext {
public:
    InstancerProviderImpl _instancerProvider;

    virtual const InstancerProvider& instancerProvider() const override {
        return _instancerProvider;
    }

    virtual InstancerProvider& instancerProvider() override {
        return _instancerProvider;
    }

    virtual const BlockPos& renderOrigin() const override {
        static BlockPos origin = BlockPos::ZERO;
        return origin;
    }
};

class TestBlockActor : public KineticBlockEntity {
public:
    static TestVisualizationContext globalVisualizationContext;

    // std::unique_ptr<ShaftVisual<TestBlockActor>> testVisuals;

    TestBlockActor(BlockActorType type, const BlockPos& pos, const std::string& id) 
        : KineticBlockEntity(type, pos, id) /*, testVisuals(nullptr)*/
    {
        Log::Info("TestBlockActor created at position: ({}, {}, {}) with id: {}", pos.x, pos.y, pos.z, id);
        mRendererId = (BlockActorRendererId)((int)BlockActorRendererId::Count + 1);
        setSpeed(16.0f);
    }

    virtual void onPlace(BlockSource& unk0) override {
        KineticBlockEntity::onPlace(unk0);

        if (mBlock == nullptr) {
            Log::Error("TestBlockActor::onPlace called but mBlock is null at position: ({}, {}, {})", mPosition.x, mPosition.y, mPosition.z);
            return;
		}

        // testVisuals = std::make_unique<ShaftVisual<TestBlockActor>>(globalVisualizationContext, this, 0.0f);
        Log::Info("TestBlockActor::onPlace called at position: ({}, {}, {})", mPosition.x, mPosition.y, mPosition.z);
    }
};

class TestBlockActorRenderer : public BlockActorRenderer {
public:
    KineticBlockEntityRenderer kineticRenderer;
    TestBlockActorRenderer() : BlockActorRenderer() {};

    virtual void render(BaseActorRenderContext& ctx, BlockActorRenderData& data) override {
        TestBlockActor& actor = static_cast<TestBlockActor&>(data.entity);
        // Log::Info("{}", actor.mBlock == nullptr ? "mBlock is null" : "mBlock is valid");
        kineticRenderer.render(*this, ctx, data);
    }
};

class RotatedPillarKineticBlock : public KineticBlock {
public:
    RotatedPillarKineticBlock(const std::string& name, short id, const Material& material)
        : KineticBlock(name, id, material) 
        {
            mBlockEntityType = JavaBlockEntity::TYPE;
        }

    virtual std::shared_ptr<BlockActor> newBlockEntity(const BlockPos& pos, const Block& block) const override {
		return std::make_shared<TestBlockActor>(JavaBlockEntity::TYPE, pos, "bosh");
	}

    const Block& getPlacementBlock(const Actor& unk0, const BlockPos& unk1, FacingID face, const Vec3& unk3, int unk4) const override {
        const Block* renderBlock = &getRenderBlock();

        Facing::Axis axis = Facing::getAxis(face);

        renderBlock = renderBlock->setState<Facing::Axis>(VanillaStates::PillarAxis, axis);

        return *renderBlock;
    }

    // NOT IN HERE, MAKE SURE TO REMOVE!!

    virtual Facing::Axis getRotationAxis(const Block& state) const {
        return state.getState<Facing::Axis>(VanillaStates::PillarAxis);
    }
};