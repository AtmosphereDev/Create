#pragma once
#include <amethyst/Imports.hpp>
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"

class SmartBlockEntity {
private:
    bool initialized;
    bool firstNbtRead;
    bool chunkUnloaded;
protected:
    int lazyTickRate;
    int lazyTickCounter;
    std::unordered_map<void*, std::shared_ptr<BlockEntityBehaviour>> behaviours;

public:
    virtual ~SmartBlockEntity() = default;

    template <typename T>
    void addBehaviour(BehaviourType<T>* type, std::shared_ptr<T> behaviour) {
        behaviours[type] = behaviour;
    }

    template <typename T>
    std::shared_ptr<T> getBehaviour(BehaviourType<T>* type) {
        auto it = behaviours.find(type);
        if (it == behaviours.end()) return nullptr;
        return std::dynamic_pointer_cast<T>(it->second);
    }

    void forEachBehaviour(const std::function<void(std::shared_ptr<BlockEntityBehaviour>)>& func) {
        for (auto& [_, behaviour] : behaviours) {
            func(behaviour);
        }
    }

    void setLazyTickRate(int rate) {
        lazyTickRate = lazyTickCounter = rate;
    }

    virtual void lazyTick() {}

    virtual void tick() {
        if (!initialized) {
            initialize();
            initialized = true;
        }

        if (--lazyTickCounter <= 0) {
            lazyTickCounter = lazyTickRate;
            lazyTick();
        }

        forEachBehaviour([](const std::shared_ptr<BlockEntityBehaviour>& b) { 
            b->tick(); 
        });
    }

    virtual void addBehaviours(std::vector<std::shared_ptr<BlockEntityBehaviour>>& behavioursList) = 0;

    virtual void initialize() {
		if (firstNbtRead) {
			firstNbtRead = false;
			// NeoForge.EVENT_BUS.post(new BlockEntityBehaviourEvent(this, behaviours)); 
            // TODO: ^
		}

		forEachBehaviour([](const std::shared_ptr<BlockEntityBehaviour>& b) { 
            b->initialize(); 
        });
		lazyTick();
	}

    void onChunkUnloaded() { chunkUnloaded = true; }

    bool isChunkUnloaded() const { return chunkUnloaded; }
};