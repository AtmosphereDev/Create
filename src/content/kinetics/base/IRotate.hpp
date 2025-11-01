#pragma once
#include <algorithm>
#include <mc/src/common/world/Facing.hpp>
#include <mc/src/common/world/level/block/Block.hpp>	
#include <mc/src/common/world/level/block/BlockLegacy.hpp>

class IRotate {
public:
    struct StressImpact {
        enum Type { LOW, MEDIUM, HIGH, OVERSTRESSED };

        static bool isEnabled() {
            return true;
            // return !AllConfigs.server().kinetics.disableStress.get(); TODO
        }
    };

    // public boolean hasShaftTowards(LevelReader world, BlockPos pos, BlockState state, Direction face);

	Facing::Axis getRotationAxis(const Block& state);

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
		return std::find(block.mTags.begin(), block.mTags.end(), HashedString("IRotate")) != block.mTags.end();
	} 
};