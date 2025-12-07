#pragma once
#include "content/kinetics/belt/behaviour/BeltProcessingBehaviour.hpp"
#include <concepts>

class ItemActor;

class PressingBehaviourSpecifics {
public:
    virtual bool tryProcessInBasin(bool simulate) = 0;
    virtual bool tryProcessOnBelt(TransportedItemStack& stack, std::vector<ItemStack> outputList, bool simulate) = 0;
    virtual bool tryProcessInWorld(ItemActor& itemEntity, bool simulate) = 0;
    virtual bool canProcessInBulk() const = 0;
    virtual void onPressingCompleted() = 0;
    virtual int getParticleAmount() const = 0;
    virtual float getKineticSpeed() const = 0;
};

template <typename T>
concept PressingCompatible =
    std::is_base_of_v<SmartBlockEntity, T> &&
    std::is_base_of_v<PressingBehaviourSpecifics, T>;

class PressingBehaviour : public BeltProcessingBehaviour {
public:
    static const int CYCLE = 240;
	static const int ENTITY_SCAN = 10;

    class Mode {
    public:
        enum Type {
            WORLD,
            BELT,
            BASIN
        };

        Type type;
        float headOffset;

        Mode(Type type) : type(type), headOffset(0.0f) {
            switch (type) {
                case WORLD:
                    headOffset = 1.0f;
                    break;
                case BELT:
                    headOffset = 19.0f / 16.0f;
                    break;
                case BASIN:
                    headOffset = 22.0f / 16.0f;
                    break;
            }
        }
    };

    std::shared_ptr<PressingBehaviourSpecifics> specifics;
	int prevRunningTicks;
	int runningTicks;
	bool running;
	bool finished;
	Mode mode;

	int entityScanCooldown;

    template <PressingCompatible T>
    PressingBehaviour(std::shared_ptr<T> be) {
        specifics = be;
        mode = Mode::WORLD;
        entityScanCooldown = ENTITY_SCAN;

        // whenItemEnters((s, i) -> BeltPressingCallbacks.onItemReceived(s, i, this));
		// whileItemHeld((s, i) -> BeltPressingCallbacks.whenItemHeld(s, i, this));
    }
};