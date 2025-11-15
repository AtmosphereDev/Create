#include "foundation/block/IBE.hpp"
#include "foundation/blockEntity/SmartBlockEntity.hpp"

template <typename T>
void IBE<T>::withBlockEntityDo(BlockSource &world, const BlockPos &pos, std::function<void(T &)> action)
{
    T* blockEntity = getBlockEntityOptional(world, pos);
    if (blockEntity != nullptr) {
        action(*blockEntity);
    }
} 

template <typename T>
inline T *IBE<T>::getBlockEntityOptional(BlockSource &world, const BlockPos &pos)
{
    BlockActor* blockEntity = world.getBlockEntity(pos);
    if (blockEntity != nullptr && SmartBlockEntity::IsSmartBlockEntity(*blockEntity)) {
        return dynamic_cast<T*>(blockEntity);
    }
    return nullptr;
}