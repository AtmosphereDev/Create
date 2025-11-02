#pragma once
#include <mc/src/common/world/level/block/actor/BlockActor.hpp>
#include <mc/src/common/world/level/block/actor/BlockActorRendererId.hpp>

class Dimension;

/**
 * A closer representation of a Java BlockEntity
 */
class JavaBlockEntity : public BlockActor {
public:
    // Not in the original code, but useful for checking if its a custom java block entity
    static BlockActorType TYPE;

    // Java actually uses level to mean a specific dimension
	Dimension* level; // Not actually in here, but its in the java BlockEntity class, maybe would be good to make a kinda middleman?

    JavaBlockEntity(BlockActorType type, const BlockPos& pos, const std::string& id)
        : BlockActor(type, pos, id), level(nullptr) {}

    void setChanged() {
        // no idea what this does, todo figure out
    }

    virtual void setRemoved() {
        // no idea what this does, todo figure out
    }

    const BlockPos& getBlockPos() const {
        return mPosition;
    }

    Dimension& getLevel() {
        return *level;
    }

    const Dimension& getLevel() const {
        return *level;
    }

    const Block& getBlock() const {
        return *mBlock;
    }
};