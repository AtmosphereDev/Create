#include "KineticBlock.hpp"
#include "foundation/block/IBE.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"

void KineticBlock::onRemove(BlockSource &blockSource, const BlockPos &pos) const
{
	Log::Info("KineticBlock::onRemove called at {}", pos);
    IBE<KineticBlockEntity>::onRemove(*this, blockSource, pos);
}

void KineticBlock::neighborChanged(BlockSource& region, const BlockPos& pos, const BlockPos& neighborPos) const
{
    if (region.isClientSide()) return;

    KineticBlockEntity* kbe = KineticBlockEntity::asKineticBlockEntity(region.getBlockEntity(pos));
    if (!kbe) return;

    if (kbe->preventSpeedUpdate > 0) {
        Log::Info("KineticBlock::neighborChanged skipping speed update at {} due to preventSpeedUpdate {}", pos, kbe->preventSpeedUpdate);
        return;
    }

    // Remove previous information when block is added
    kbe->warnOfMovement();
    kbe->clearKineticInformation();
    kbe->updateSpeed = true;
    Log::Info("KineticBlock::neighborChanged called at {}", pos);\
}
