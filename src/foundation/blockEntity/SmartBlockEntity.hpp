#pragma once
#include <amethyst/Imports.hpp>
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"
#include "foundation/blockEntity/CachedRenderBBBlockEntity.hpp"

class SmartBlockEntity : public CachedRenderBBBlockEntity {
private:
    bool initialized;
    bool firstNbtRead;
    bool chunkUnloaded;
protected:
    int lazyTickRate;
    int lazyTickCounter;
    std::unordered_map<void*, std::shared_ptr<BlockEntityBehaviour>> behaviours;
    bool virtualMode;

public:
    SmartBlockEntity(BlockActorType typeIn, const BlockPos& pos, const std::string& id)
        : CachedRenderBBBlockEntity(typeIn, pos, id), initialized(false) {
            setLazyTickRate(10);

            // original calls addBehaviours here, but that doesnt work in c++..
        }

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

    virtual void tick(BlockSource& source) override {
        if (!initialized) {
            // Jank fix for JavaBlockEntity
            if (!level) {
                level = source.mDimension;
                mBlock = &source.getBlock(mPosition);
            }

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
        // Original one does this in constructor, but that doesnt work in c++
        std::vector<std::shared_ptr<BlockEntityBehaviour>> list;
        addBehaviours(list);
        //list.forEach(b -> behaviours.put(b.getType(), b));
        for (auto& behaviour : list) {
            Log::Info("Adding behaviour to SmartBlockEntity not impl!");
        }

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

    virtual void setRemoved() override {
        CachedRenderBBBlockEntity::setRemoved();
        if (!chunkUnloaded)
            remove();

        invalidate();
    }

    /**
	 * Block destroyed or Chunk unloaded. Usually invalidates capabilities
	 */
	void invalidate() {
        forEachBehaviour([](const std::shared_ptr<BlockEntityBehaviour>& b) { 
            b->unload(); 
        });
	}

    /**
	 * Block destroyed or picked up by a contraption. Usually detaches kinetics
	 */
	virtual void remove() {}

    /**
	 * Block destroyed or replaced. Requires Block to call IBE::onRemove
	 */
	void destroy() {
        forEachBehaviour([](const std::shared_ptr<BlockEntityBehaviour>& b) { 
            b->destroy(); 
        });
	}

    void onChunkUnloaded() { chunkUnloaded = true; }

    void markVirtual() {
		virtualMode = true;
	}

    bool isVirtual() {
		return virtualMode;
	}

    bool isChunkUnloaded() const { return chunkUnloaded; }
};