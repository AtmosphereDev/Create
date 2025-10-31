#pragma once

class IRotate {
public:
    struct StressImpact {
        enum Type { LOW, MEDIUM, HIGH, OVERSTRESSED };

        static bool isEnabled() {
            return true;
            // return !AllConfigs.server().kinetics.disableStress.get(); TODO
        }
    }

    // public boolean hasShaftTowards(LevelReader world, BlockPos pos, BlockState state, Direction face);

	// public Axis getRotationAxis(BlockState state);

	// public default SpeedLevel getMinimumRequiredSpeedLevel() {
	// 	return SpeedLevel.NONE;
	// }

	// public default boolean hideStressImpact() {
	// 	return false;
	// }

	// public default boolean showCapacityWithAnnotation() {
	// 	return false;
	// }
};