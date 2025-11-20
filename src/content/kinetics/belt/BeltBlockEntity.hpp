#pragma once
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include <mc/src/common/nbt/CompoundTagVariant.hpp>
#include <mc/src/common/nbt/CompoundTag.hpp>
#include "content/kinetics/belt/behaviour/TransportedItemStackHandlerBehaviour.hpp"
#include "content/kinetics/belt/transport/BeltInventory.hpp"
#include "amethyst/game/capabilities/IItemHandler.hpp"
#include "amethyst/runtime/events/RegisterCapabilitiesEvent.hpp"

class BeltInventory;
class TransportedItemStack;

class BeltBlockEntity : public KineticBlockEntity {
public:
    // Map<Entity, TransportedEntityInfo> passengers;
	// Optional<DyeColor> color;
    int beltLength;
	int index;
	FacingID lastInsert;
	// CasingType casing;
	bool covered;

	std::optional<BlockPos> controller;
	std::shared_ptr<BeltInventory> inventory;
	std::shared_ptr<IItemHandler> itemHandler;
	// publicVersionedInventoryTrackerBehaviour invVersionTracker;

	CompoundTag trackerUpdateTag;

    BeltBlockEntity(const BlockPos& pos, const std::string& id)
        : KineticBlockEntity(pos, id), controller(std::nullopt), beltLength(0), inventory(nullptr) {}

    virtual void addBehaviours(std::vector<std::shared_ptr<BlockEntityBehaviour>>& behavioursList) override {
        KineticBlockEntity::addBehaviours(behavioursList);
        // behaviours.add(new DirectBeltInputBehaviour(this).onlyInsertWhen(this::canInsertFrom)
		// 	.setInsertionHandler(this::tryInsertingFromSide).considerOccupiedWhen(this::isOccupied));

		auto transportedItemStackHandlerBehaviour = std::make_shared<TransportedItemStackHandlerBehaviour>(
			this, 
			[this](
				float maxDistanceFromCenter,
				std::function<std::optional<TransportedItemStackHandlerBehaviour::TransportedResult>(TransportedItemStack &)> process
			) {
				this->applyToAllItems(maxDistanceFromCenter, process);
			}
		);	

		transportedItemStackHandlerBehaviour->withStackPlacement(
			[this](const TransportedItemStack& stack){ return getWorldPositionOf(stack); }
		);

		behavioursList.push_back(transportedItemStackHandlerBehaviour);

		// behaviours.add(invVersionTracker = new VersionedInventoryTrackerBehaviour(this));
    }

	static void registerCapabilities(RegisterCapabilitiesEvent& event);

	static void AddEventListeners() {
		Amethyst::GetEventBus().AddListener<RegisterCapabilitiesEvent>(registerCapabilities);
	}

    virtual void tick(BlockSource& source) override;

	virtual float calculateStressApplied() override {
		if (!isController())
			return 0.0f;
		return KineticBlockEntity::calculateStressApplied();
	}

	void initializeItemHandler();

	virtual void destroy() override;

	virtual void invalidate() override;

	virtual void write(CompoundTag& compound, BlockSource& region) override;

	virtual void read(const CompoundTag& compound, BlockSource& region) override;

	virtual void clearKineticInformation() override {
		KineticBlockEntity::clearKineticInformation();
		beltLength = 0;
		index = 0;
		controller = std::nullopt;
		// trackerUpdateTag = new CompoundTag();
	}

	BeltBlockEntity* getControllerBE() const;

	void setController(const BlockPos& controllerPos) {
		controller = controllerPos;
	}

	std::optional<BlockPos> getController() const {
		return controller;
	}

	bool isController() const {
		return controller.has_value() && controller.value() == mPosition;
	}

	float getBeltMovementSpeed() const {
		return getSpeed() / 480.0f;
	}

	float getDirectionAwareBeltMovementSpeed() const;

	bool hasPulley() const;

	bool isLastBelt() const;

	BlockPos getMovementDirection(bool firstHalf) const;

	BlockPos getBeltChainDirection() const;

	BlockPos getMovementDirection(bool firstHalf, bool ignoreHalves) const;

	FacingID getMovementFacing() const;

	FacingID getBeltFacing() const;

	std::shared_ptr<BeltInventory> getInventory();

	void applyToAllItems(float maxDistanceFromCenter, const std::function<std::optional<TransportedItemStackHandlerBehaviour::TransportedResult>(TransportedItemStack &)>& func);
	
	Vec3 getWorldPositionOf(const TransportedItemStack& stack) const;

	// setCasingType

	bool canInsertFrom(FacingID side);

	bool isOccupied(FacingID side);

	ItemStack tryInsertingFromSide(TransportedItemStack&& stack, FacingID side, bool simulate);

	virtual bool canPropagateDiagonally(IRotate& block, const Block& state) override;

	virtual float propagateRotationTo(KineticBlockEntity& target, const Block& stateFrom, const Block& stateTo, const BlockPos& diff,
									 bool connectedViaAxes, bool connectedViaCogs) override;

	void invalidateItemHandler();

	// setCovered
};