#pragma once
#include <amethyst/runtime/ModContext.hpp>
#include <amethyst/runtime/utility/InlineHook.hpp>
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

    JavaBlockEntity(const BlockPos& pos, const std::string& id)
        : BlockActor(JavaBlockEntity::TYPE, pos, id), level(nullptr) {
            mRendererId = (BlockActorRendererId)((int)BlockActorRendererId::Count + 1);
        }

    void setChanged() {
        // i assume this is all the java impl does too
        mChanged = true;
    }

    virtual void onChanged(BlockSource& region) override {
        Log::Info("JavaBlockEntity::onChanged at {}", mPosition);
        region.fireBlockChanged(mPosition, 0, *mBlock, *mBlock, 3, BlockChangedEventTarget::SelfBlock, nullptr, nullptr);
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

    virtual void afterConstructed() {
        // Called directly after a block entity is spawned
    }

    virtual bool save(CompoundTag& unk0) const override {
        Log::Info("JavaBlockEntity::save at {}", mPosition);
        unk0.putByte("test", 0);
        return true;
    }

    virtual void load(Level& unk0, const CompoundTag& unk1, DataLoadHelper& unk2) override {
        Log::Info("JavaBlockEntity::load {} at {}", unk1.getByte("test"), mPosition);
    }

    void _initializeAfterBlockEntityConstructed(LevelChunk& lc) {
        Log::Info("_initializeAfterBlockEntityConstructed");
        level = lc.mDimension;
		mBlock = &level->mBlockSource->getBlock(mPosition);
		afterConstructed();
    }

    static bool IsJavaBlockEntity(const BlockActor& actor) {
        return actor.mType == TYPE;
	}

    virtual bool _isSmartBlockEntity() const {
        return false;
    }
};

void JavaBlockEntityHooks();