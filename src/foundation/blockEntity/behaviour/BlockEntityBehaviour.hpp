#pragma once
#include "foundation/blockEntity/behaviour/BehaviourType.hpp"
#include "foundation/blockEntity/SmartBlockEntity.hpp"

class SmartBlockEntity;
class Level;
class Block;
class Dimension;
class CompoundTag;
class BlockActor;

class BlockEntityBehaviour {
protected:
    SmartBlockEntity* blockEntity = nullptr;
    int lazyTickRate = 10;
    int lazyTickCounter = 10;

public:
    explicit BlockEntityBehaviour(SmartBlockEntity* be) : blockEntity(be) {}
    virtual ~BlockEntityBehaviour() = default;

    virtual const BehaviourType& getType() const = 0;

    virtual void initialize() {}

    virtual void tick() {
        if (--lazyTickCounter <= 0) {
            lazyTickCounter = lazyTickRate;
            lazyTick();
        }
    }

    virtual void read(const CompoundTag& nbt, bool clientPacket) {

	}

	virtual void write(CompoundTag& nbt, bool clientPacket) {

	}

    /**
     * Called when isSafeNBT == true. Defaults to write()
     */
	void writeSafe(CompoundTag& nbt) {
		write(nbt, false);
	}

	virtual bool isSafeNBT() {
		return false;
	}

	// public ItemRequirement getRequiredItems() {
	// 	return ItemRequirement.NONE;
	// }

    virtual void onBlockChanged(const Block& oldState) {

	}

	virtual void onNeighborChanged(const BlockPos& neighborPos) {

	}

    /**
	 * Block destroyed or Chunk unloaded. Usually invalidates capabilities
	 */
	virtual void unload() {
	}

	/**
	 * Block destroyed or removed. Requires block to call ITE::onRemove
	 */
	virtual void destroy() {
	}

    void setLazyTickRate(int slowTickRate) {
        this->lazyTickRate = slowTickRate;
        this->lazyTickCounter = slowTickRate;
    }

    virtual void lazyTick() {}

    BlockPos getPos() const;

    Dimension* getWorld() const;

    template <typename T>
    static std::shared_ptr<T> get(const BlockSource& region, const BlockPos& pos, const BehaviourType& type) {
        return get(region.getBlockEntity(pos), type);
    }

    template <typename T>
    static std::shared_ptr<T> get(BlockActor* be, const BehaviourType& type) {
        SmartBlockEntity* sbe = SmartBlockEntity::AsSmartBlockEntity(be);
        if (!sbe) return nullptr;
        return sbe->getBehaviour<T>(type);
    }
};