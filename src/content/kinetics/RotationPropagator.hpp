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

    static bool isLargeToLargeGear(const Block& from, const Block& to, const BlockPos& diff) {
		return false;
		// if (!ICogWheel.isLargeCog(from) || !ICogWheel.isLargeCog(to))
		// 	return false;
		// Axis fromAxis = from.getValue(AXIS);
		// Axis toAxis = to.getValue(AXIS);
		// if (fromAxis == toAxis)
		// 	return false;
		// for (Axis axis : Axis.values()) {
		// 	int axisDiff = axis.choose(diff.getX(), diff.getY(), diff.getZ());
		// 	if (axis == fromAxis || axis == toAxis) {
		// 		if (axisDiff == 0)
		// 			return false;

		// 	} else if (axisDiff != 0)
		// 		return false;
		// }
		// return true;
	}

    static float getAxisModifier(KineticBlockEntity& be, FacingID direction) {
		return 1.0f; // TODO

	// 	if (!(be.hasSource() || be.isSource()) || !(be instanceof DirectionalShaftHalvesBlockEntity))
	// 		return 1;
	// 	Direction source = ((DirectionalShaftHalvesBlockEntity) be).getSourceFacing();

	// 	if (be instanceof GearboxBlockEntity)
	// 		return direction.getAxis() == source.getAxis() ? direction == source ? 1 : -1
	// 			: direction.getAxisDirection() == source.getAxisDirection() ? -1 : 1;

	// 	if (be instanceof SplitShaftBlockEntity)
	// 		return ((SplitShaftBlockEntity) be).getRotationSpeedModifier(direction);

	// 	return 1;
	}

	static bool isLargeToSmallCog(const Block& from, const Block& to, IRotate& defTo, const BlockPos& diff) {
		return false;
	// 	Axis axisFrom = from.getValue(AXIS);
	// 	if (axisFrom != defTo.getRotationAxis(to))
	// 		return false;
	// 	if (axisFrom.choose(diff.getX(), diff.getY(), diff.getZ()) != 0)
	// 		return false;
	// 	for (Axis axis : Axis.values()) {
	// 		if (axis == axisFrom)
	// 			continue;
	// 		if (Math.abs(axis.choose(diff.getX(), diff.getY(), diff.getZ())) != 1)
	// 			return false;
	// 	}
	// 	return true;
	}

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

        if (!worldIn.mBlockSource->areChunksFullyLoaded(pos, 1))
            return;

		propagateNewSource(addedTE);
	}

	/**
	 * Search for sourceless networks attached to the given entity and update them.
	 *
	 * @param currentTE
	 */
	static void propagateNewSource(KineticBlockEntity& currentTE);

	// /**
	//  * Remove the given entity from the network.
	//  *
	//  * @param worldIn
	//  * @param pos
	//  * @param removedBE
	//  */
	static void handleRemoved(const Dimension& worldIn, const BlockPos& pos, KineticBlockEntity& removedBE) {
	// 	if (worldIn.isClientSide)
	// 		return;
	// 	if (removedBE == null)
	// 		return;
	// 	if (removedBE.getTheoreticalSpeed() == 0)
	// 		return;

	// 	for (BlockPos neighbourPos : getPotentialNeighbourLocations(removedBE)) {
	// 		BlockState neighbourState = worldIn.getBlockState(neighbourPos);
	// 		if (!(neighbourState.getBlock() instanceof IRotate))
	// 			continue;
	// 		BlockEntity blockEntity = worldIn.getBlockEntity(neighbourPos);
	// 		if (!(blockEntity instanceof KineticBlockEntity neighbourBE))
	// 			continue;

	// 		if (!neighbourBE.hasSource() || !neighbourBE.source.equals(pos))
	// 			continue;

	// 		propagateMissingSource(neighbourBE);
	// 	}

	}

	// /**
	//  * Clear the entire subnetwork depending on the given entity and find a new
	//  * source
	//  *
	//  * @param updateTE
	//  */
	static void propagateMissingSource(KineticBlockEntity& updateTE) {
	// 	final Level world = updateTE.getLevel();

	// 	List<KineticBlockEntity> potentialNewSources = new LinkedList<>();
	// 	List<BlockPos> frontier = new LinkedList<>();
	// 	frontier.add(updateTE.getBlockPos());
	// 	BlockPos missingSource = updateTE.hasSource() ? updateTE.source : null;

	// 	while (!frontier.isEmpty()) {
	// 		final BlockPos pos = frontier.remove(0);
	// 		BlockEntity blockEntity = world.getBlockEntity(pos);
	// 		if (!(blockEntity instanceof KineticBlockEntity currentBE))
	// 			continue;

	// 		currentBE.removeSource();
	// 		currentBE.sendData();

	// 		for (KineticBlockEntity neighbourBE : getConnectedNeighbours(currentBE)) {
	// 			if (neighbourBE.getBlockPos()
	// 				.equals(missingSource))
	// 				continue;
	// 			if (!neighbourBE.hasSource())
	// 				continue;

	// 			if (!neighbourBE.source.equals(pos)) {
	// 				potentialNewSources.add(neighbourBE);
	// 				continue;
	// 			}

	// 			if (neighbourBE.isSource())
	// 				potentialNewSources.add(neighbourBE);

	// 			frontier.add(neighbourBE.getBlockPos());
	// 		}
	// 	}

	// 	for (KineticBlockEntity newSource : potentialNewSources) {
	// 		if (newSource.hasSource() || newSource.isSource()) {
	// 			propagateNewSource(newSource);
	// 			return;
	// 		}
	// 	}
	}

	static KineticBlockEntity* findConnectedNeighbour(KineticBlockEntity& currentTE, const BlockPos& neighbourPos);

	static bool isConnected(KineticBlockEntity& from, KineticBlockEntity& to);

	static std::vector<KineticBlockEntity*> getConnectedNeighbours(KineticBlockEntity& be);

	static std::vector<BlockPos> getPotentialNeighbourLocations(KineticBlockEntity& be);
};