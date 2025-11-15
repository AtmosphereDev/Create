#pragma once
#include "content/kinetics/base/HorizontalKineticBlock.hpp"
#include <mc/src/common/world/level/block/BlockState.hpp>
#include "content/kinetics/belt/BeltSlope.hpp"
#include "content/kinetics/belt/BeltPart.hpp"   
#include "foundation/block/IBE.hpp"
#include "content/kinetics/belt/BeltBlockEntity.hpp"

class BeltBlock : public HorizontalKineticBlock, public IBE<BeltBlockEntity> {
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

    virtual std::shared_ptr<BlockActor> newBlockEntity(const BlockPos& pos, const Block& block) const override;

    Facing::Axis getRotationAxis(const Block& state) const override {
        if (state.getState<BeltSlope::Type>(SLOPE()) == BeltSlope::SIDEWAYS) {
            return Facing::Axis::Y;
        }
        return Facing::getAxis(
            Facing::getClockWise(state.getState<FacingID>(HORIZONTAL_FACING()))
        );
    }

    const Block& getPlacementBlock(const Actor& unk0, const BlockPos& unk1, FacingID face, const Vec3& unk3, int unk4) const override {
        const Block* renderBlock = &getRenderBlock();
        renderBlock = renderBlock->setState<BeltPart::Type>(SLOPE(), BeltPart::PULLEY);
        return *renderBlock;
    }

    virtual void entityInside(BlockSource& unk0, const BlockPos& unk1, Actor& unk2) override;

    virtual AABB getCollisionShape(const Block& block, const BlockSource& region, const BlockPos& at, optional_ref<const GetCollisionShapeInterface> shapeInterface) const override;

    static void initBelt(BlockSource& region, const BlockPos& pos);

    static std::vector<BlockPos> getBeltChain(BlockSource& region, const BlockPos& startPos);

    static std::optional<BlockPos> nextSegmentPosition(const Block& state, const BlockPos& _pos, bool forward);

    static bool canTransportObjects(const Block& state);
};