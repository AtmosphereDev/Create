#include "BeltHelper.hpp"
#include "content/kinetics/belt/BeltBlockEntity.hpp"
#include "content/kinetics/belt/BeltBlock.hpp"

BlockPos BeltHelper::getPositionForOffset(BeltBlockEntity *controller, int offset)
{
    const BlockPos& pos = controller->getBlockPos();
    BlockPos vec = Facing::normal(controller->getBeltFacing()); // original uses Vec3i
    auto slope = controller->getBlock().getState<BeltSlope::Type>(BeltBlock::SLOPE());
    int verticality = slope == BeltSlope::DOWNWARD ? -1 : slope == BeltSlope::UPWARD ? 1 : 0;

    return pos.offset(offset * vec.x, std::clamp(offset, 0, controller->beltLength - 1) * verticality, offset * vec.z);
}