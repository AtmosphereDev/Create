#pragma once
#include <mc/src/common/world/level/block/BlockLegacy.hpp>
#include <mc/src/common/world/level/BlockSource.hpp>

class IBE {
public:
    static void onRemove(const BlockLegacy& block, BlockSource& blockSource, const BlockPos& pos);
};