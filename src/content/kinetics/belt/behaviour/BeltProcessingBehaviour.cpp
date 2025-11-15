#include "BeltProcessingBehaviour.hpp"

const BehaviourType BeltProcessingBehaviour::TYPE = BehaviourType("BeltProcessingBehaviour");

bool BeltProcessingBehaviour::isBlocked(Dimension &world, const BlockPos &processingSpace)
{
    const Block& block = world.mBlockSource->getBlock(processingSpace.above());
    // if (AbstractFunnelBlock.isFunnel(blockState))
	// 		return false;

    AABB out;
    return block.getCollisionShape(out, *world.mBlockSource, processingSpace.above(), nullptr);
}