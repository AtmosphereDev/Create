#pragma once
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include <mc/src/common/nbt/CompoundTagVariant.hpp>
#include <mc/src/common/nbt/CompoundTag.hpp>

class BeltBlockEntity : public KineticBlockEntity {
public:
    // Map<Entity, TransportedEntityInfo> passengers;
	// Optional<DyeColor> color;
    int beltLength;
	int index;
	FacingID lastInsert;
	// CasingType casing;
	bool covered;

	BlockPos controller;
	// BeltInventory inventory;
	// IItemHandler itemHandler;
	// publicVersionedInventoryTrackerBehaviour invVersionTracker;

	CompoundTag trackerUpdateTag;

    BeltBlockEntity(const BlockPos& pos, const std::string& id)
        : KineticBlockEntity(pos, id), controller(BlockPos::ZERO), beltLength(0)
        {}

    virtual void addBehaviours(std::vector<std::shared_ptr<BlockEntityBehaviour>>& behavioursList) override {
        KineticBlockEntity::addBehaviours(behavioursList);
        // behaviours.add(new DirectBeltInputBehaviour(this).onlyInsertWhen(this::canInsertFrom)
		// 	.setInsertionHandler(this::tryInsertingFromSide).considerOccupiedWhen(this::isOccupied));
		// behaviours.add(new TransportedItemStackHandlerBehaviour(this, this::applyToAllItems)
		// 	.withStackPlacement(this::getWorldPositionOf));
		// behaviours.add(invVersionTracker = new VersionedInventoryTrackerBehaviour(this));
    }

    virtual void tick(BlockSource& source) override;

	bool hasPulley() const;
};