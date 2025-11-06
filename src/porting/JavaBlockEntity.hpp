#pragma once
#include <mc/src/common/world/level/block/actor/BlockActor.hpp>
#include <mc/src/common/world/level/block/actor/BlockActorRendererId.hpp>
#include <mc/src/common/world/level/BlockSource.hpp>
#include <mc/src/common/world/level/chunk/LevelChunk.hpp>
#include <mc/src/common/world/level/dimension/Dimension.hpp>

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
        : BlockActor(type, pos, id), level(nullptr) {
            mRendererId = (BlockActorRendererId)((int)BlockActorRendererId::Count + 1);
        }

    virtual void onPlace(BlockSource& region) override {
        level = region.mDimension;
        mBlock = &region.getBlock(mPosition);
        BlockActor::onPlace(region);
    }

    virtual void fixupOnLoad(LevelChunk& unk0) override {
		Log::Info("JavaBlockEntity::fixupOnLoad called");
        level = unk0.mDimension;
		mBlock = &level->mBlockSource->getBlock(mPosition);
		BlockActor::fixupOnLoad(unk0);
    }

    void setChanged() {
        // no idea what this does, todo figure out
    }

    virtual void setRemoved() {
        // no idea what this does, todo figure out
    }

    const BlockPos& getBlockPos() const {
        return mPosition;
    }

    bool hasLevel() const {
        return level != nullptr;
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