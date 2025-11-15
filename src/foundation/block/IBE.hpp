#pragma once
#include <mc/src/common/world/level/block/BlockLegacy.hpp>
#include <mc/src/common/world/level/BlockSource.hpp>
#include "foundation/blockEntity/SmartBlockEntity.hpp"

template <typename T>
class IBE {
public:
    static void onRemove(const BlockLegacy& block, BlockSource& blockSource, const BlockPos& pos) 
    {
        if (!block.hasBlockEntity()) return;

        // BlockLegacy::onRemove isnt passed the new block state, so cant check this
        // ^ actually could use block source ig, but seems unnnecessary
        // if (blockState.is(newBlockState.getBlock()) && newBlockState.hasBlockEntity())
        // 	return;

        const BlockActor* blockEntity = blockSource.getBlockEntity(pos);
        if (blockEntity != nullptr && SmartBlockEntity::IsSmartBlockEntity(*blockEntity)) {
            SmartBlockEntity* smartBE = static_cast<SmartBlockEntity*>(const_cast<BlockActor*>(blockEntity));
            smartBE->destroy();
        }

        // handled automatically i think
        // level.removeBlockEntity(pos);
    }

    void withBlockEntityDo(BlockSource& world, const BlockPos& pos, std::function<void(T&)> action);

    T* getBlockEntityOptional(BlockSource& world, const BlockPos& pos);
};

#include "foundation/block/IBE.inl"