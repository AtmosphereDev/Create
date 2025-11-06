#pragma once
#include "foundation/blockEntity/behaviour/BehaviourType.hpp"

class SmartBlockEntity;
class Level;
class Block;
class Dimension;

class BlockEntityBehaviour {
protected:
    SmartBlockEntity* blockEntity = nullptr;
    int lazyTickRate = 10;
    int lazyTickCounter = 10;

public:
    explicit BlockEntityBehaviour(SmartBlockEntity* be) : blockEntity(be) {}
    virtual ~BlockEntityBehaviour() = default;

    // virtual BehaviourType<BlockEntityBehaviour>* getType() const = 0;

    virtual void initialize() {}

    virtual void tick() {
        if (--lazyTickCounter <= 0) {
            lazyTickCounter = lazyTickRate;
            lazyTick();
        }
    }

    // public void read(CompoundTag nbt, HolderLookup.Provider registries, boolean clientPacket) {

	// }

	// public void write(CompoundTag nbt, HolderLookup.Provider registries, boolean clientPacket) {

	// }

    /**
     * Called when isSafeNBT == true. Defaults to write()
     */
	// public void writeSafe(CompoundTag nbt, HolderLookup.Provider registries) {
	// 	write(nbt, registries, false);
	// }

	bool isSafeNBT() {
		return false;
	}

	// public ItemRequirement getRequiredItems() {
	// 	return ItemRequirement.NONE;
	// }

    void onBlockChanged(const Block& oldState) {

	}

	void onNeighborChanged(const BlockPos& neighborPos) {

	}

    /**
	 * Block destroyed or Chunk unloaded. Usually invalidates capabilities
	 */
	void unload() {
	}

	/**
	 * Block destroyed or removed. Requires block to call ITE::onRemove
	 */
	void destroy() {
	}

    void setLazyTickRate(int slowTickRate) {
        this->lazyTickRate = slowTickRate;
        this->lazyTickCounter = slowTickRate;
    }

    virtual void lazyTick() {}

    BlockPos getPos() const;

    Dimension* getWorld() const;
};