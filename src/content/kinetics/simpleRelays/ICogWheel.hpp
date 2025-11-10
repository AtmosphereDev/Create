#pragma once
#include "content/kinetics/base/IRotate.hpp"

class ICogWheel {
public:
    static bool isICogWheel(const Block& block) {
		return std::find(block.mLegacyBlock->mTags.begin(), block.mLegacyBlock->mTags.end(), HashedString("ICogWheel")) != block.mLegacyBlock->mTags.end();
	} 

    static bool isSmallCog(const Block& block) {
        //return isICogWheel(block) && ((ICogWheel*)block.mLegacyBlock.get())->isSmallCog();
        return true;
    }

    static bool isLargeCog(const Block& block) {
        
        return false;
        //return isICogWheel(block) && ((ICogWheel*)block.mLegacyBlock.get())->isLargeCog();
    }

    static bool isDedicatedCogWheel(const Block& block) {
        return isICogWheel(block) && ((ICogWheel*)block.mLegacyBlock.get())->isDedicatedCogWheel();
    }

    virtual bool isLargeCog() {
        return false;
    }

    virtual bool isSmallCog() {
        return !isLargeCog();
    }

    virtual bool isDedicatedCogWheel() {
        return false;
    }
};