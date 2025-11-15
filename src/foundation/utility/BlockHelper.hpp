#pragma once
#include "mc/src/common/world/level/block/Block.hpp"
#include "mc/src/common/world/level/BlockSource.hpp"

class BlockHelper {
public:
    static bool hasBlockSolidSide(const Block& state, BlockSource& region, const BlockPos& pos, FacingID side) {
        // return !state.is(BlockTags.LEAVES)
		// 	&& Block.isFaceFull(state.getCollisionShape(blockGetter, pos), dir);

        // Todo: Make this use actual logic
        return !state.isAir() && !state.canBeBuiltOver(region, pos); // rough impl
    }
};