#pragma once
#include "content/kinetics/base/IRotate.hpp"

class ICogWheel {
public:
    static bool isICogWheel(const Block& block) {
		return std::find(block.mLegacyBlock->mTags.begin(), block.mLegacyBlock->mTags.end(), HashedString("ICogWheel")) != block.mLegacyBlock->mTags.end();
	} 

    static bool isSmallCog(const Block& block) {
        const ICogWheel* cogWheel = asICogWheel(block.mLegacyBlock.get());
        if (!cogWheel) return false;
        return cogWheel->isSmallCog();
    }

    static bool isLargeCog(const Block& block) {
        const ICogWheel* cogWheel = asICogWheel(block.mLegacyBlock.get());
        if (!cogWheel) return false;
        return cogWheel->isLargeCog();
    }

    static const ICogWheel* asICogWheel(const BlockLegacy* blockLegacy) {
        if (std::find(blockLegacy->mTags.begin(), blockLegacy->mTags.end(), HashedString("ICogWheel")) != blockLegacy->mTags.end()) {
            return dynamic_cast<const ICogWheel*>(blockLegacy);
        }
        return nullptr;
    }

    static bool isDedicatedCogWheel(const Block& block) {
        return isICogWheel(block) && ((ICogWheel*)block.mLegacyBlock.get())->isDedicatedCogWheel();
    }

    virtual bool isLargeCog() const {
        return false;
    }

    virtual bool isSmallCog() const {
        return !isLargeCog();
    }

    virtual bool isDedicatedCogWheel() const {
        return false;
    }
};