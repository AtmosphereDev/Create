#include "BeltBlockEntity.hpp"
#include "BeltBlock.hpp"
#include "AllBlocks.hpp"

void BeltBlockEntity::tick(BlockSource &source)
 {
    // jank fix
    if (level == nullptr) return;

    if (beltLength == 0) {
        BeltBlock::initBelt(source, mPosition);
        //Log::Info("BeltBlockEntity tick initialized belt at {}", mPosition);
    }
}

bool BeltBlockEntity::hasPulley() const
{
    if (AllBlocks::BELT != getBlock().mLegacyBlock) 
        return false;
    return getBlock().getState<BeltPart::Type>(BeltBlock::PART()) != BeltPart::MIDDLE;
}