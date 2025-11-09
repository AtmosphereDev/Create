#pragma once
#include <amethyst/Imports.hpp>
#include "foundation/blockEntity/CachedRenderBBBlockEntity.hpp"
#include "foundation/blockEntity/behaviour/BehaviourType.hpp"

class BlockEntityBehaviour;

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
        : CachedRenderBBBlockEntity(typeIn, pos, id), initialized(false), firstNbtRead(false), chunkUnloaded(false), lazyTickRate(0), lazyTickCounter(0), virtualMode(false) {
            setLazyTickRate(10);
        }

    virtual ~SmartBlockEntity() = default;

    virtual void afterConstructed() override;

    template <typename T>
    void addBehaviour(const BehaviourType& type, std::shared_ptr<T> behaviour);

    template <typename T>
    std::shared_ptr<T> getBehaviour(const BehaviourType& type);

    void forEachBehaviour(const std::function<void(std::shared_ptr<BlockEntityBehaviour>)>& func) {
        for (auto& [_, behaviour] : behaviours) {
            func(behaviour);
        }
    }

    const std::unordered_map<BehaviourType, std::shared_ptr<BlockEntityBehaviour>>& getAllBehaviours() const {
        return behaviours;
    }

    void setLazyTickRate(int rate) {
        lazyTickRate = lazyTickCounter = rate;
    }

    virtual void lazyTick() {}

    virtual void tick(BlockSource& source) override;

    virtual void addBehaviours(std::vector<std::shared_ptr<BlockEntityBehaviour>>& behavioursList) = 0;

    virtual void initialize();

    // weirdly the original code has no load function?
    virtual void onChunkUnloaded(LevelChunk& unk0) override {
        CachedRenderBBBlockEntity::onChunkUnloaded(unk0);
        chunkUnloaded = true;
		Log::Info("SmartBlockEntity onChunkUnloaded called at {}", mPosition);
    }

    virtual void onRemoved(BlockSource& region) override {
        CachedRenderBBBlockEntity::onRemoved(region);
        if (!chunkUnloaded) remove();
        else {
            Log::Info("SmartBlockEntity onRemoved did not call remove() due to chunkUnloaded at {}", mPosition);
        }
            

        invalidate();
    }

    /**
	 * Block destroyed or Chunk unloaded. Usually invalidates capabilities
	 */
	void invalidate();

    /**
	 * Block destroyed or picked up by a contraption. Usually detaches kinetics
	 */
	virtual void remove() {
		Log::Info("SmartBlockEntity::remove at {}", mPosition);
    }

    /**
	 * Block destroyed or replaced. Requires Block to call IBE::onRemove
	 */
	void destroy();

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

    static const SmartBlockEntity* AsSmartBlockEntity(const BlockActor* actor) {
        if (!actor || !IsSmartBlockEntity(*actor)) return nullptr;
        return static_cast<const SmartBlockEntity*>(actor);
    }

    static SmartBlockEntity* AsSmartBlockEntity(BlockActor* actor) {
        if (!actor || !IsSmartBlockEntity(*actor)) return nullptr;
        return static_cast<SmartBlockEntity*>(actor);
    }
};

#include "SmartBlockEntity.inl"