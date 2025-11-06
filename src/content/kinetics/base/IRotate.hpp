#pragma once
#include <algorithm>
#include <mc/src/common/world/Facing.hpp>
#include <mc/src/common/world/level/block/Block.hpp>	
#include <mc/src/common/world/level/block/BlockLegacy.hpp>
#include "porting/JavaBlockEntity.hpp"

class IRotate : public BlockLegacy {
public:
    struct StressImpact {
        enum Type { LOW, MEDIUM, HIGH, OVERSTRESSED };

        static bool isEnabled() {
            return true;
            // return !AllConfigs.server().kinetics.disableStress.get(); TODO
        }
    };

	struct SpeedLevel {
		enum Type { NONE, SLOW, MEDIUM, FAST };

		Type type;
		SpeedLevel(Type type) : type(type) {}

		static SpeedLevel of(float speed);

		bool operator==(const SpeedLevel& other) const { return type == other.type; }
    	bool operator!=(const SpeedLevel& other) const { return type != other.type; }
		bool operator==(Type other) const { return type == other; }
    	bool operator!=(Type other) const { return type != other; }
	};

	IRotate(const std::string& name, short id, const Material& material)
		: BlockLegacy(name, id, material) {
			mBlockEntityType = JavaBlockEntity::TYPE;
			mTags.push_back(HashedString("IRotate"));
		}

    virtual bool hasShaftTowards(Dimension& world, const BlockPos& pos, const Block& state, FacingID face) = 0;

	virtual Facing::Axis getRotationAxis(const Block& state) const = 0;

	// public default SpeedLevel getMinimumRequiredSpeedLevel() {
	// 	return SpeedLevel.NONE;
	// }

	// public default boolean hideStressImpact() {
	// 	return false;
	// }

	// public default boolean showCapacityWithAnnotation() {
	// 	return false;
	// }

	static bool isIRotate(const Block& block) {
		// Idk a better way to do this rn
		return std::find(block.mLegacyBlock->mTags.begin(), block.mLegacyBlock->mTags.end(), HashedString("IRotate")) != block.mLegacyBlock->mTags.end();
	} 
};