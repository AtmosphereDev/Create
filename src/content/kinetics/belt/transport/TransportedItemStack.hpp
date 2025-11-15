#pragma once
#include <mc/src/common/world/item/ItemStack.hpp>
#include "porting/Random.hpp"
#include "content/kinetics/belt/BeltHelper.hpp"

class TransportedItemStack {
public:

    ItemStack stack;
    float beltPosition;
    float sideOffset;
    int angle;
    int insertedAt;
    FacingID insertedFrom;
    bool locked;
    bool lockedExternally;

    float prevBeltPosition;
    float prevSideOffset;

    // FanProcessingType processedBy
    int processingTime;

    TransportedItemStack(ItemStack stack)
        : stack(stack) {
            bool centered = BeltHelper::isItemUpright(stack);
            angle = centered ? 180 : Random::nextInt(360);
            // if (PackageItem.isPackage(stack))
			//     angle = Random::nextInt(4) * 90 + Random::nextInt(20) - 10;
            sideOffset = prevSideOffset = getTargetSideOffset();
            insertedFrom = FacingID::UP;
        }

    float getTargetSideOffset() const {
        return (angle - 180) / (360.0f * 3.0f);
    }

    int compareTo(const TransportedItemStack& other) const {
        return beltPosition < other.beltPosition ? 1 : beltPosition > other.beltPosition ? -1 : 0;
    }
};