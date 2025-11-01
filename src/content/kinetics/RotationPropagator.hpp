#pragma once
#include <mc/src/common/world/level/dimension/Dimension.hpp>
#include <mc/src/common/world/level/BlockSource.hpp>
#include <mc/src/common/world/Facing.hpp>

class KineticBlockEntity;

class RotationPropagator {
public:
    static int MAX_FLICKER_SCORE;

    static float getRotationSpeedModifier(KineticBlockEntity& from, KineticBlockEntity& to) {
        return 0.0f; // TODO

		// final BlockState stateFrom = from.getBlockState();
		// final BlockState stateTo = to.getBlockState();

		// Block fromBlock = stateFrom.getBlock();
		// Block toBlock = stateTo.getBlock();
		// if (!(fromBlock instanceof IRotate definitionFrom && toBlock instanceof IRotate definitionTo))
		// 	return 0;

		// final BlockPos diff = to.getBlockPos()
		// 	.subtract(from.getBlockPos());
		// final Direction direction = Direction.getNearest(diff.getX(), diff.getY(), diff.getZ());
		// final Level world = from.getLevel();

		// boolean alignedAxes = true;
		// for (Axis axis : Axis.values())
		// 	if (axis != direction.getAxis())
		// 		if (axis.choose(diff.getX(), diff.getY(), diff.getZ()) != 0)
		// 			alignedAxes = false;

		// boolean connectedByAxis =
		// 	alignedAxes && definitionFrom.hasShaftTowards(world, from.getBlockPos(), stateFrom, direction)
		// 		&& definitionTo.hasShaftTowards(world, to.getBlockPos(), stateTo, direction.getOpposite());

		// boolean connectedByGears = ICogWheel.isSmallCog(stateFrom)
		// 	&& ICogWheel.isSmallCog(stateTo);

		// float custom = from.propagateRotationTo(to, stateFrom, stateTo, diff, connectedByAxis, connectedByGears);
		// if (custom != 0)
		// 	return custom;

		// // Axis <-> Axis
		// if (connectedByAxis) {
		// 	float axisModifier = getAxisModifier(to, direction.getOpposite());
		// 	if (axisModifier != 0)
		// 		axisModifier = 1 / axisModifier;
		// 	return getAxisModifier(from, direction) * axisModifier;
		// }

		// // Attached Encased Belts
		// if (fromBlock instanceof ChainDriveBlock && toBlock instanceof ChainDriveBlock) {
		// 	boolean connected = ChainDriveBlock.areBlocksConnected(stateFrom, stateTo, direction);
		// 	return connected ? ChainDriveBlock.getRotationSpeedModifier(from, to) : 0;
		// }

		// // Large Gear <-> Large Gear
		// if (isLargeToLargeGear(stateFrom, stateTo, diff)) {
		// 	Axis sourceAxis = stateFrom.getValue(AXIS);
		// 	Axis targetAxis = stateTo.getValue(AXIS);
		// 	int sourceAxisDiff = sourceAxis.choose(diff.getX(), diff.getY(), diff.getZ());
		// 	int targetAxisDiff = targetAxis.choose(diff.getX(), diff.getY(), diff.getZ());

		// 	return sourceAxisDiff > 0 ^ targetAxisDiff > 0 ? -1 : 1;
		// }

		// // Gear <-> Large Gear
		// if (ICogWheel.isLargeCog(stateFrom) && ICogWheel.isSmallCog(stateTo))
		// 	if (isLargeToSmallCog(stateFrom, stateTo, definitionTo, diff))
		// 		return -2f;
		// if (ICogWheel.isLargeCog(stateTo) && ICogWheel.isSmallCog(stateFrom))
		// 	if (isLargeToSmallCog(stateTo, stateFrom, definitionFrom, diff))
		// 		return -.5f;

		// // Gear <-> Gear
		// if (connectedByGears) {
		// 	if (diff.distManhattan(BlockPos.ZERO) != 1)
		// 		return 0;
		// 	if (ICogWheel.isLargeCog(stateTo))
		// 		return 0;
		// 	if (direction.getAxis() == definitionFrom.getRotationAxis(stateFrom))
		// 		return 0;
		// 	if (definitionFrom.getRotationAxis(stateFrom) == definitionTo.getRotationAxis(stateTo))
		// 		return -1;
		// }
        //
		// return 0;
	}

    static float getConveyedSpeed(KineticBlockEntity& from, KineticBlockEntity& to) {
        return 0.0f; // TODO

		// final BlockState stateFrom = from.getBlockState();
		// final BlockState stateTo = to.getBlockState();

		// // Rotation Speed Controller <-> Large Gear
		// if (isLargeCogToSpeedController(stateFrom, stateTo, to.getBlockPos()
		// 	.subtract(from.getBlockPos())))
		// 	return SpeedControllerBlockEntity.getConveyedSpeed(from, to, true);
		// if (isLargeCogToSpeedController(stateTo, stateFrom, from.getBlockPos()
		// 	.subtract(to.getBlockPos())))
		// 	return SpeedControllerBlockEntity.getConveyedSpeed(to, from, false);

		// float rotationSpeedModifier = getRotationSpeedModifier(from, to);
		// return from.getTheoreticalSpeed() * rotationSpeedModifier;
	}

    // static bool isLargeToLargeGear(BlockState from, BlockState to, BlockPos diff) {
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
	// }

    // static float getAxisModifier(KineticBlockEntity be, Direction direction) {
	// 	if (!(be.hasSource() || be.isSource()) || !(be instanceof DirectionalShaftHalvesBlockEntity))
	// 		return 1;
	// 	Direction source = ((DirectionalShaftHalvesBlockEntity) be).getSourceFacing();

	// 	if (be instanceof GearboxBlockEntity)
	// 		return direction.getAxis() == source.getAxis() ? direction == source ? 1 : -1
	// 			: direction.getAxisDirection() == source.getAxisDirection() ? -1 : 1;

	// 	if (be instanceof SplitShaftBlockEntity)
	// 		return ((SplitShaftBlockEntity) be).getRotationSpeedModifier(direction);

	// 	return 1;
	// }

    // private static boolean isLargeToSmallCog(BlockState from, BlockState to, IRotate defTo, BlockPos diff) {
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
	// }

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
	// public static void handleRemoved(Level worldIn, BlockPos pos, KineticBlockEntity removedBE) {
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

	// }

	// /**
	//  * Clear the entire subnetwork depending on the given entity and find a new
	//  * source
	//  *
	//  * @param updateTE
	//  */
	// private static void propagateMissingSource(KineticBlockEntity updateTE) {
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
	// }

	static KineticBlockEntity* findConnectedNeighbour(KineticBlockEntity& currentTE, const BlockPos& neighbourPos);

	static bool isConnected(KineticBlockEntity& from, KineticBlockEntity& to);

	static std::vector<KineticBlockEntity*> getConnectedNeighbours(KineticBlockEntity& be);

	static std::vector<BlockPos> getPotentialNeighbourLocations(KineticBlockEntity& be);
};