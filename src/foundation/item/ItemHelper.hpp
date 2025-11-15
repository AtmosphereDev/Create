#pragma once
#include <mc/src/common/world/item/ItemStack.hpp>
#include <mc/src/common/world/item/Item.hpp>    
#include <mc/src/common/world/entity/components/ActorDefinitionIdentifierComponent.hpp>
#include <mc/src/common/world/actor/item/ItemActor.hpp>

class ItemHelper {
public:
    static ItemStack limitCountToMaxStackSize(ItemStack& stack, bool simulate) {
        int count = stack.mCount;
        int max = stack.mItem->mMaxStackSize;
        if (count <= max)
            return ItemStack::EMPTY_ITEM;

        ItemStack remainder = stack.copyWithCount(count - max);
        if (!simulate) {
            stack.mCount = max;
        }
        return remainder;
    }

    static ItemStack fromItemEntity(Actor& actor) {
        if (!actor.isAlive()) {
            return ItemStack::EMPTY_ITEM;
        }

        // if (entityIn instanceof PackageEntity packageEntity) {
		// 	return packageEntity.getBox();
		// }

        ActorDefinitionIdentifierComponent* component = actor.tryGetComponent<ActorDefinitionIdentifierComponent>();

        if (!component || component->mIdentifier.mCanonicalName != HashedString{"minecraft:item"}) {
            return ItemStack::EMPTY_ITEM;
        }

        ItemActor& itemActor = (ItemActor&)actor;
        return itemActor.mItem;
    }
};