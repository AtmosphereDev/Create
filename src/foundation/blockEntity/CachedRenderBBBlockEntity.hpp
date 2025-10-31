#pragma once
#include <optional>
#include <mc/src/common/world/phys/AABB.hpp>
#include "foundation/blockEntity/SynchedBlockEntity.hpp"

class CachedRenderBBBlockEntity : public SynchedBlockEntity {
private:
    std::optional<AABB> renderBoundingBox;

public:
    // public CachedRenderBBBlockEntity(BlockEntityType<?> type, BlockPos pos, BlockState state) {
    //     super(type, pos, state);
    // }

    // const AABB& getRenderBoundingBox() {
    //     if (renderBoundingBox == null) {
    //         renderBoundingBox = createRenderBoundingBox();
    //     }
    //     return renderBoundingBox;
    // }

    // void invalidateRenderBoundingBox() {
    //     renderBoundingBox = null;
    // }

    // AABB createRenderBoundingBox() {
    //     return new AABB(getBlockPos());
    // }   
};