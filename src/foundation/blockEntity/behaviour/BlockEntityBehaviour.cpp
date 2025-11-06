#include "BlockEntityBehaviour.hpp"
#include "foundation/blockEntity/SmartBlockEntity.hpp"

BlockPos BlockEntityBehaviour::getPos() const
{
    return blockEntity->mPosition;
}

Dimension *BlockEntityBehaviour::getWorld() const
{
    return blockEntity->level;
}
