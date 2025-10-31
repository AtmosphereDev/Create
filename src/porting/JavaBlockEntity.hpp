#pragma once
#include <mc/src/common/world/level/block/actor/BlockActor.hpp>

class Dimension;

/**
 * A closer representation of a Java BlockEntity
 */
class JavaBlockEntity : public BlockActor {
public:
    // Java actually uses level to mean a specific dimension
	Dimension* level; // Not actually in here, but its in the java BlockEntity class, maybe would be good to make a kinda middleman?

    void setChanged() {
        // no idea what this does, todo figure out
    }

    virtual void setRemoved() {
        // no idea what this does, todo figure out
    }
};