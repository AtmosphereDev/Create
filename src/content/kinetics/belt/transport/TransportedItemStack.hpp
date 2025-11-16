#pragma once
#include <mc/src/common/world/item/ItemStack.hpp>
#include "porting/Random.hpp"
#include "content/kinetics/belt/BeltHelper.hpp"
#include <mc/src/common/nbt/CompoundTagVariant.hpp>
#include <mc/src/common/nbt/CompoundTag.hpp>

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
        : stack(stack), beltPosition(0.0f), sideOffset(0.0f), angle(0), insertedAt(0), insertedFrom(FacingID::UP),
          locked(false), lockedExternally(false), prevBeltPosition(0.0f), prevSideOffset(0.0f), processingTime(0) {
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

    std::unique_ptr<CompoundTag> serializeNBT() const {
        std::unique_ptr<CompoundTag> nbt = std::make_unique<CompoundTag>();
        nbt->put("Item", stack.save());
        nbt->putFloat("Pos", beltPosition);
        nbt->putFloat("PrevPos", prevBeltPosition);
        nbt->putFloat("Offset", sideOffset);
        nbt->putFloat("PrevOffset", prevSideOffset);
        nbt->putInt("InSegment", insertedAt);
        nbt->putInt("Angle", angle);
        nbt->putInt("InDirection", (int)insertedFrom);
        // if (processedBy != null) {
		// 	ResourceLocation key = CreateBuiltInRegistries.FAN_PROCESSING_TYPE.getKey(processedBy);
		// 	if (key == null)
		// 		throw new IllegalArgumentException("Could not get id for FanProcessingType " + processedBy + "!");

		// 	nbt.putString("FanProcessingType", key.toString());
		// 	nbt.putInt("FanProcessingTime", processingTime);
		// }

        if (locked)
            nbt->putByte("Locked", 1);
        if (lockedExternally)
            nbt->putByte("LockedExternally", 1);
        return nbt;
    }

    static TransportedItemStack deserializeNBT(const CompoundTag& nbt) {
        ItemStack item = ItemStack();
        const CompoundTag* itemTag = nbt.getCompound("Item");
        if (itemTag != nullptr) {
            item._loadItem(itemTag);
        }

        TransportedItemStack stack(item);
        stack.beltPosition = nbt.getFloat("Pos");
        stack.prevBeltPosition = nbt.getFloat("PrevPos");
        stack.sideOffset = nbt.getFloat("Offset");
        stack.prevSideOffset = nbt.getFloat("PrevOffset");
        stack.insertedAt = nbt.getInt("InSegment");
        stack.angle = nbt.getInt("Angle");
        stack.insertedFrom = static_cast<FacingID>(nbt.getInt("InDirection"));

        // if (nbt.contains("FanProcessingType", Tag::Type::String)) {

        return stack;
    }
};