#include "RotationPropagator.hpp"
#include <mc/src-deps/core/math/Math.hpp>
#include "infrastructure/config/AllConfigs.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include "content/kinetics/simpleRelays/ICogWheel.hpp"
#include <mc/src/common/world/level/block/VanillaStates.hpp>

float RotationPropagator::getRotationSpeedModifier(KineticBlockEntity& from, KineticBlockEntity& to)
{
    const Block& stateFrom = from.getBlock();
    const Block& stateTo = to.getBlock();

    const IRotate* definitionFrom = IRotate::asIRotate(stateFrom.mLegacyBlock.get());
    const IRotate* definitionTo = IRotate::asIRotate(stateTo.mLegacyBlock.get());

    if (definitionFrom == nullptr || definitionTo == nullptr)
        return 0.0f;

    BlockPos diff = to.getBlockPos() - from.getBlockPos();
    FacingID direction = Facing::getNearest(diff);
    const Dimension& world = from.getLevel();

    bool alignedAxis = true;
    for (const Facing::Axis& axis : Facing::AXES) {
        if (axis != Facing::getAxis(direction)) {
            if (Facing::choose(axis, diff.x, diff.y, diff.z) != 0) {
                alignedAxis = false;
                break;
            }
        }
    }

    bool connectedByAxis = alignedAxis && definitionFrom->hasShaftTowards(world, from.getBlockPos(), stateFrom, direction)
        && definitionTo->hasShaftTowards(world, to.getBlockPos(), stateTo, Facing::getOpposite(direction));

    bool connectedByGears = ICogWheel::isSmallCog(stateFrom) && ICogWheel::isSmallCog(stateTo);

    float custom = from.propagateRotationTo(to, stateFrom, stateTo, diff, connectedByAxis, connectedByGears);
    if (custom != 0.0f) return custom;
    
    // Axis <-> Axis
    if (connectedByAxis) {
        float axisModifier = getAxisModifier(to, Facing::getOpposite(direction));
        if (axisModifier != 0) {
            axisModifier = 1 / axisModifier;
        }

        return getAxisModifier(from, direction) * axisModifier;
    }

    // Attached encased belts
    // if (fromBlock instanceof ChainDriveBlock && toBlock instanceof ChainDriveBlock) {
    //     boolean connected = ChainDriveBlock.areBlocksConnected(stateFrom, stateTo, direction);
    //     return connected ? ChainDriveBlock.getRotationSpeedModifier(from, to) : 0;
    // }

    // Large Gear <-> Large Gear
    if (isLargeToLargeGear(stateFrom, stateTo, diff)) {
        Facing::Axis sourceAxis = stateFrom.getState<Facing::Axis>(VanillaStates::PillarAxis);
        Facing::Axis targetAxis = stateTo.getState<Facing::Axis>(VanillaStates::PillarAxis);
        int sourceAxisDiff = Facing::choose(sourceAxis, diff.x, diff.y, diff.z);
        int targetAxisDiff = Facing::choose(targetAxis, diff.x, diff.y, diff.z);
        return sourceAxisDiff > 0 ^ targetAxisDiff > 0 ? -1.0f : 1.0f;
    }

    // Gear <-> Large Gear
    if (ICogWheel::isLargeCog(stateFrom) && ICogWheel::isSmallCog(stateTo))
        if (isLargeToSmallCog(stateFrom, stateTo, *definitionTo, diff))
            return -2.0f;
    if (ICogWheel::isLargeCog(stateTo) && ICogWheel::isSmallCog(stateFrom))
        if (isLargeToSmallCog(stateTo, stateFrom, *definitionFrom, diff))
            return -0.5f;

    // Gear <-> Gear
    if (connectedByGears) {
        if (diff.distManhattan(BlockPos::ZERO) != 1)
            return 0;
        if (ICogWheel::isLargeCog(stateTo))
            return 0;
        if (Facing::getAxis(direction) == definitionFrom->getRotationAxis(stateFrom))
            return 0;
        if (definitionFrom->getRotationAxis(stateFrom) == definitionTo->getRotationAxis(stateTo))
            return -1;
    }

    return 0.0f;
}

float RotationPropagator::getConveyedSpeed(KineticBlockEntity &from, KineticBlockEntity &to)
{
    // Rotation Speed Controller <-> Large Gear
    // if (isLargeCogToSpeedController(stateFrom, stateTo, to.getBlockPos()
    //     .subtract(from.getBlockPos())))
    //     return SpeedControllerBlockEntity.getConveyedSpeed(from, to, true);
    // if (isLargeCogToSpeedController(stateTo, stateFrom, from.getBlockPos()
    //     .subtract(to.getBlockPos())))
    //     return SpeedControllerBlockEntity.getConveyedSpeed(to, from, false);

    float rotationSpeedModifier = getRotationSpeedModifier(from, to);
    return from.getTheoreticalSpeed() * rotationSpeedModifier;
}

bool RotationPropagator::isLargeToLargeGear(const Block &from, const Block &to, const BlockPos &diff)
{
    if (!ICogWheel::isLargeCog(from) || !ICogWheel::isLargeCog(to))
        return false;

    Facing::Axis fromAxis = from.getState<Facing::Axis>(VanillaStates::PillarAxis);
    Facing::Axis toAxis = to.getState<Facing::Axis>(VanillaStates::PillarAxis);
    if (fromAxis == toAxis)
        return false;

    for (const auto& axis : Facing::AXES) {
        int axisDiff = Facing::choose(axis, diff.x, diff.y, diff.z);
        if (axis == fromAxis || axis == toAxis) {
            if (axisDiff == 0) 
                return false;
        }
        else if (axisDiff != 0) {
            return false;
        }
    }
    return true;
}

bool RotationPropagator::isLargeToSmallCog(const Block &from, const Block &to, const IRotate &defTo, const BlockPos &diff)
{
    Facing::Axis axisFrom = from.getState<Facing::Axis>(VanillaStates::PillarAxis);
    if (axisFrom != defTo.getRotationAxis(to))
        return false;
    if (Facing::choose(axisFrom, diff.x, diff.y, diff.z) != 0)
        return false;
    for (const auto& axis : Facing::AXES) {
        if (axis == axisFrom)
            continue;
        if (std::abs(Facing::choose(axis, diff.x, diff.y, diff.z)) != 1)
            return false;
    }
    return true;
}

void RotationPropagator::propagateNewSource(KineticBlockEntity &currentTE)
{
    BlockPos pos = currentTE.getBlockPos();
    Dimension& world = currentTE.getLevel();

    for (KineticBlockEntity* neighbourTE : getConnectedNeighbours(currentTE)) {
        float speedOfCurrent = currentTE.getTheoreticalSpeed();
        float speedOfNeighbour = neighbourTE->getTheoreticalSpeed();
        float newSpeed = getConveyedSpeed(currentTE, *neighbourTE);
        float oppositeSpeed = getConveyedSpeed(*neighbourTE, currentTE);

        if (newSpeed == 0 && oppositeSpeed == 0)
            continue;

        bool incompatible =
            mce::Math::signum(newSpeed) != mce::Math::signum(speedOfNeighbour) && (newSpeed != 0 && speedOfNeighbour != 0);

        bool tooFast = mce::Math::abs(newSpeed) > AllConfigs::server().kinetics.maxRotationSpeed.get()
            || mce::Math::abs(oppositeSpeed) > AllConfigs::server().kinetics.maxRotationSpeed.get();
            // Check for both the new speed and the opposite speed, just in case
            
        bool speedChangedTooOften = currentTE.getFlickerScore() > MAX_FLICKER_SCORE;
        if (tooFast || speedChangedTooOften) {
            world.destroyBlock(pos, true);
            return;
        }

        // Opposite directions
        if (incompatible) {
            world.destroyBlock(pos, true);
            return;

            // Same direction: overpower the slower speed
        } else {

            // Neighbour faster, overpower the incoming tree
            if (mce::Math::abs(oppositeSpeed) > mce::Math::abs(speedOfCurrent)) {
                float prevSpeed = currentTE.getSpeed();
                currentTE.setSource(neighbourTE->getBlockPos());
                currentTE.setSpeed(getConveyedSpeed(*neighbourTE, currentTE));
                currentTE.onSpeedChanged(prevSpeed);
                currentTE.sendData();

                propagateNewSource(currentTE);
                return;
            }

            // Current faster, overpower the neighbours' tree
            if (mce::Math::abs(newSpeed) >= mce::Math::abs(speedOfNeighbour)) {

                // Do not overpower you own network -> cycle
                if (!currentTE.hasNetwork() || currentTE.network == neighbourTE->network) {
                    float epsilon = mce::Math::abs(speedOfNeighbour) / 256.0f / 256.0f;
                    if (mce::Math::abs(newSpeed) > mce::Math::abs(speedOfNeighbour) + epsilon)
                        world.destroyBlock(pos, true);
                    continue;
                }

                if (currentTE.hasSource() && currentTE.source == neighbourTE->getBlockPos())
                    currentTE.removeSource();

                float prevSpeed = neighbourTE->getSpeed();
                neighbourTE->setSource(currentTE.getBlockPos());
                neighbourTE->setSpeed(getConveyedSpeed(currentTE, *neighbourTE));
                neighbourTE->onSpeedChanged(prevSpeed);
                neighbourTE->sendData();
                propagateNewSource(*neighbourTE);
                continue;
            }
        }

        if (neighbourTE->getTheoreticalSpeed() == newSpeed)
            continue;

        float prevSpeed = neighbourTE->getSpeed();
        neighbourTE->setSpeed(newSpeed);
        neighbourTE->setSource(currentTE.getBlockPos());
        neighbourTE->onSpeedChanged(prevSpeed);
        neighbourTE->sendData();
        propagateNewSource(*neighbourTE);
    }
}

KineticBlockEntity* RotationPropagator::findConnectedNeighbour(KineticBlockEntity &currentTE, const BlockPos &neighbourPos)
{
    const BlockSource& region = *currentTE.getLevel().mBlockSource;

    const Block& neighbourState = region.getBlock(neighbourPos);

    if (!IRotate::isIRotate(neighbourState)) 
        return nullptr;

    const BlockActor* neighborActor = region.getBlockEntity(neighbourPos);
    if (!neighborActor) 
        return nullptr;

    if (!KineticBlockEntity::isKineticBlockEntity(*neighborActor))
        return nullptr;

    KineticBlockEntity& neighbourKBE = (KineticBlockEntity&)(*neighborActor);

    // Idk why this wouldnt ever be true, but its in original source
    if (!IRotate::isIRotate(neighbourKBE.getBlock())) 
        return nullptr;

    if (!isConnected(currentTE, neighbourKBE) && !isConnected(neighbourKBE, currentTE))
        return nullptr;
        
    return &neighbourKBE;
}

bool RotationPropagator::isConnected(KineticBlockEntity &from, KineticBlockEntity &to)
 {
    const Block& stateFrom = from.getBlock();
    const Block& stateTo = to.getBlock();
    
    return isLargeCogToSpeedController(stateFrom, stateTo, to.getBlockPos() - from.getBlockPos()) 
        || getRotationSpeedModifier(from, to) != 0
        || from.isCustomConnection(to, stateFrom, stateTo);
}

std::vector<KineticBlockEntity *> RotationPropagator::getConnectedNeighbours(KineticBlockEntity &be)
 {
    std::vector<KineticBlockEntity*> neighbours;
    for (BlockPos neighbourPos : getPotentialNeighbourLocations(be)) {
        KineticBlockEntity* neighbourBE = findConnectedNeighbour(be, neighbourPos);
        if (neighbourBE == nullptr)
            continue;

        neighbours.push_back(neighbourBE);
    }
    return neighbours;
}

std::vector<BlockPos> RotationPropagator::getPotentialNeighbourLocations(KineticBlockEntity &be)
{
    std::vector<BlockPos> neighbours;
    BlockPos blockPos = be.getBlockPos();
    Dimension& level = be.getLevel();

    if (!level.mBlockSource->areChunksFullyLoaded(blockPos, 1))
        return neighbours;

    for (FacingID facing : Facing::DIRECTIONS) {
        BlockPos relative = blockPos.neighbor(facing);
        if (level.mBlockSource->areChunksFullyLoaded(relative, 1))
            neighbours.push_back(relative);
    }

    const Block& block = be.getBlock();
    if (!IRotate::isIRotate(block))
        return neighbours;

    IRotate* rotate = static_cast<IRotate*>(block.mLegacyBlock.get());
    return be.addPropagationLocations(*rotate, block, neighbours);
}