#pragma once
#include "BlockEntityBehaviour.hpp"
#include <mc/src/common/world/level/BlockSource.hpp>
#include "foundation/blockEntity/SmartBlockEntity.hpp"

template <typename T>
static std::shared_ptr<T> BlockEntityBehaviour::get(const BlockSource& region, const BlockPos& pos, const BehaviourType& type) {
    return get<T>(region.getBlockEntity(pos), type);
}

template <typename T>
static std::shared_ptr<T> BlockEntityBehaviour::get(BlockActor* be, const BehaviourType& type) {
    SmartBlockEntity* sbe = SmartBlockEntity::AsSmartBlockEntity(be);
    if (!sbe) return nullptr;
    return sbe->getBehaviour<T>(type);
}