#include "RotationPropagator.hpp"
#include <mc/src-deps/core/math/Math.hpp>
#include "infrastructure/config/AllConfigs.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"

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