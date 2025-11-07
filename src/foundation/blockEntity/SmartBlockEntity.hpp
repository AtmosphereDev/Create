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
    std::unordered_map<BehaviourType, std::shared_ptr<BlockEntityBehaviour>> behaviours;
    bool virtualMode;

public:
    SmartBlockEntity(BlockActorType typeIn, const BlockPos& pos, const std::string& id)
        : CachedRenderBBBlockEntity(typeIn, pos, id), initialized(false) {
            setLazyTickRate(10);
        }

    virtual ~SmartBlockEntity() = default;

    virtual void afterConstructed() override {
        CachedRenderBBBlockEntity::afterConstructed();
        std::vector<std::shared_ptr<BlockEntityBehaviour>> list;

        addBehaviours(list);

        for (auto& behaviour : list) {
			addBehaviour(behaviour->getType(), behaviour);
        }
    }

    template <typename T>
    void addBehaviour(const BehaviourType& type, std::shared_ptr<T> behaviour) {
        behaviours[type] = behaviour;
    }

    template <typename T>
    std::shared_ptr<T> getBehaviour(const BehaviourType& type) {
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

        CachedRenderBBBlockEntity::tick(source);
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

    // weirdly the original code has no load function?
    virtual void onChunkUnloaded(LevelChunk& unk0) override {
        CachedRenderBBBlockEntity::onChunkUnloaded(unk0);
        chunkUnloaded = true;
    }

    virtual void onRemoved(BlockSource& region) override {
        CachedRenderBBBlockEntity::onRemoved(region);
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

    virtual bool _isSmartBlockEntity() const final {
        return true;
    }

    static bool IsSmartBlockEntity(const BlockActor& actor) {
        if (!JavaBlockEntity::IsJavaBlockEntity(actor)) return false;
        return static_cast<const JavaBlockEntity&>(actor)._isSmartBlockEntity();
    }
};