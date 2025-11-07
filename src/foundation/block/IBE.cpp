#include "IBE.hpp"
#include "foundation/blockEntity/SmartBlockEntity.hpp"

void IBE::onRemove(const BlockLegacy &block, BlockSource &blockSource, const BlockPos &pos)
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