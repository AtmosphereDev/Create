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

BeltBlockEntity *BeltHelper::getSegmentBE(BlockSource &region, const BlockPos &pos)
{
    return dynamic_cast<BeltBlockEntity*>(region.getBlockEntity(pos));
}

Vec3 BeltHelper::getVectorForOffset(BeltBlockEntity *controller, float offset)
{
    BeltSlope::Type slope = controller->getBlock().getState<BeltSlope::Type>(BeltBlock::SLOPE());
    int verticality = slope == BeltSlope::DOWNWARD ? -1 : slope == BeltSlope::UPWARD ? 1 : 0;
    float verticalMovement = verticality;
    if (offset < 0.5f) 
        verticalMovement = 0;
    verticalMovement = verticalMovement * (std::min(offset, controller->beltLength - 0.5f) - 0.5f);
    Vec3 vec = controller->getBlockPos().center();
    Vec3 horizontalMovement = Vec3::atLowerCornerOf(Facing::normal(controller->getBeltFacing())) * (offset - 0.5f);

    if (slope == BeltSlope::VERTICAL)
        horizontalMovement = Vec3::ZERO;
    
    vec = vec + horizontalMovement + Vec3(0, verticalMovement, 0);
    return vec;
}
