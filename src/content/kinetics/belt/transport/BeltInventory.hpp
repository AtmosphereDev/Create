#pragma once
#include "TransportedItemStack.hpp"
#include "content/kinetics/belt/behaviour/TransportedItemStackHandlerBehaviour.hpp"

class BeltBockEntity;
class BeltProcessingBehaviour;
class TransportedItemStackHandlerBehaviour;
class BeltBlockEntity;

class BeltInventory {
public:
    BeltBlockEntity* belt;
    std::vector<TransportedItemStack> items;
    std::vector<TransportedItemStack> toInsert;
    std::vector<TransportedItemStack> toRemove;
    bool beltMovementPositive;
    float SEGMENT_WINDOW = 0.75f;

    std::optional<TransportedItemStack> lazyClientItem;

    BeltInventory(BeltBlockEntity* belt)
        : belt(belt) {}

    void tick();

    bool handleBeltProcessingAndCheckIfRemoved(TransportedItemStack& currentItem, float nextOffset, bool noMovement);

    std::shared_ptr<BeltProcessingBehaviour> getBeltProcessingAtSegment(int segment);

    std::shared_ptr<TransportedItemStackHandlerBehaviour> getTransportedItemStackHandlerAtSegment(int segment);

public:
    class Ending {
    public:
        enum Type {
            UNRESOLVED,
            EJECT,
            INSERT,
            FUNNEL,
            BLOCKED
        };

        Type type;
        float margin;

        Ending(Type type) {
            this->type = type;

            if (type == UNRESOLVED) margin = 0.0f;
            else if (type == EJECT) margin = 0.0f;
            else if (type == INSERT) margin = 0.25f;
            else if (type == FUNNEL) margin = 0.5f;
            else if (type == BLOCKED) margin = 0.45f;
        }

        bool operator==(Type type) const {
            return this->type == type;
        }
    };

    Ending resolveEnding();

public:
    bool canInsertAt(int segment);

    bool canInsertAtFromSide(int segment, FacingID side);

    bool isBlocking(int segment, FacingID side, float segmentPos, TransportedItemStack& stack);

    void addItem(TransportedItemStack&& stack);

    void insert(TransportedItemStack&& stack);

    TransportedItemStack* getStackAtOffset(int offset);

    void read(const CompoundTag& nbt);

    CompoundTag write() const;

    void eject(TransportedItemStack& stack);

    void ejectAll();

    void applyToEachWithin(float position, float maxDistanceToPosition, std::function<TransportedItemStackHandlerBehaviour::TransportedResult(TransportedItemStack&)> func);

    std::vector<TransportedItemStack>& getTransportedItems() {
        return items;
    }

    std::optional<TransportedItemStack>& getLazyClientItem() {
        return lazyClientItem;
    }
};