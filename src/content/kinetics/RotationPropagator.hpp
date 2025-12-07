#pragma once
#include <mc/src/common/world/level/dimension/Dimension.hpp>
#include <mc/src/common/world/level/BlockSource.hpp>
#include <mc/src/common/world/Facing.hpp>

class KineticBlockEntity;
class IRotate;

class RotationPropagator {
public:
    static const int MAX_FLICKER_SCORE = 128;

    static float getRotationSpeedModifier(KineticBlockEntity& from, KineticBlockEntity& to);

    static float getConveyedSpeed(KineticBlockEntity& from, KineticBlockEntity& to);

    static bool isLargeToLargeGear(const Block& from, const Block& to, const BlockPos& diff);

    static float getAxisModifier(KineticBlockEntity& be, FacingID direction);

	static bool isLargeToSmallCog(const Block& from, const Block& to, const IRotate& defTo, const BlockPos& diff);

	static bool isLargeCogToSpeedController(const Block& from, const Block& to, const BlockPos& diff) {
		return false;
		// if (!ICogWheel.isLargeCog(from) || !AllBlocks.ROTATION_SPEED_CONTROLLER.has(to))
		// 	return false;
		// if (!diff.equals(BlockPos.ZERO.below()))
		// 	return false;
		// Axis axis = from.getValue(CogWheelBlock.AXIS);
		// if (axis.isVertical())
		// 	return false;
		// if (to.getValue(SpeedControllerBlock.HORIZONTAL_AXIS) == axis)
		// 	return false;
		// return true;
	}

	/**
	 * Insert the added position to the kinetic network.
	 *
	 * @param worldIn
	 * @param pos
	 */
	static void handleAdded(Dimension& worldIn, BlockPos pos, KineticBlockEntity& addedTE) {
		if (worldIn.isClientSide())
			return;

        if (!worldIn.mBlockSource->areChunksFullyLoaded(pos, 1)) {
			Log::Info("Chunks not fully loaded around {}, skipping handleAdded", pos);
			return;
		}

		propagateNewSource(addedTE);
	}

	/**
	 * Search for sourceless networks attached to the given entity and update them.
	 *
	 * @param currentTE
	 */
	static void propagateNewSource(KineticBlockEntity& currentTE);

	/**
	 * Remove the given entity from the network.
	 *
	 * @param worldIn
	 * @param pos
	 * @param removedBE
	 */
	static void handleRemoved(const Dimension& worldIn, const BlockPos& pos, KineticBlockEntity* removedBE);

	/**
	 * Clear the entire subnetwork depending on the given entity and find a new
	 * source
	 *
	 * @param updateTE
	 */
	static void propagateMissingSource(const KineticBlockEntity& updateTE);

	static KineticBlockEntity* findConnectedNeighbour(KineticBlockEntity& currentTE, const BlockPos& neighbourPos);

	static bool isConnected(KineticBlockEntity& from, KineticBlockEntity& to);

	static std::vector<KineticBlockEntity*> getConnectedNeighbours(KineticBlockEntity& be);

	static std::vector<BlockPos> getPotentialNeighbourLocations(KineticBlockEntity& be);
};