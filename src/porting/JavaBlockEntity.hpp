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

    // Stores a weak to itself, useful for passing to other things as a std::shared_ptr
    std::weak_ptr<JavaBlockEntity> weakThis;

    JavaBlockEntity(const BlockPos& pos, const std::string& id)
        : BlockActor(JavaBlockEntity::TYPE, pos, id), level(nullptr), weakThis() {
            mRendererId = (BlockActorRendererId)((int)BlockActorRendererId::Count + 1);
        }

    void setChanged() {
        // i assume this is all the java impl does too
        mChanged = true;
    }

    virtual void onChanged(BlockSource& region) override {
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
        std::shared_ptr<BlockActor> baseShared = this->findSharedFromRegion(*level->mBlockSource);
        std::shared_ptr<JavaBlockEntity> shared = std::dynamic_pointer_cast<JavaBlockEntity>(baseShared);
        weakThis = shared;

        if (weakThis.expired()) {
            Log::Warning("JavaBlockEntity::afterConstructed weakThis is expired at {}, this is bad!", mPosition);
        }
    }

    template <typename T = JavaBlockEntity>
    std::shared_ptr<T> getShared() {
        auto shared = weakThis.lock();

        if (!shared) {
            Log::Warning("JavaBlockEntity::getShared called but weakThis is expired at {}", mPosition);
            return nullptr;
        }

        return std::dynamic_pointer_cast<T>(shared);
    }

    virtual bool save(CompoundTag& unk0) const override {
        return true;
    }

    virtual void load(Level& unk0, const CompoundTag& unk1, DataLoadHelper& unk2) override {}

    void _initializeAfterBlockEntityConstructed(LevelChunk& lc) {
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

    void invalidateCapabilities() {
        // Seems to be a Forge specific thing for making non hardcoded behaviours, hmmm not sure how to port this
    }
};

void JavaBlockEntityHooks();