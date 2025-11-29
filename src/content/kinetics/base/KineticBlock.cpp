#include "KineticBlock.hpp"
#include "foundation/block/IBE.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"

void KineticBlock::onRemove(BlockSource &blockSource, const BlockPos &pos) const
{
    IBE<KineticBlockEntity>::onRemove(*this, blockSource, pos);
}

void KineticBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const
{
    if (region.isClientSide()) return;

    KineticBlockEntity* kbe = KineticBlockEntity::asKineticBlockEntity(region.getBlockEntity(pos));
    if (!kbe) return;

    if (kbe->preventSpeedUpdate > 0) {
        return;
    }

    // Remove previous information when block is added
    kbe->warnOfMovement();
    kbe->clearKineticInformation();
    kbe->updateSpeed = true;
}
