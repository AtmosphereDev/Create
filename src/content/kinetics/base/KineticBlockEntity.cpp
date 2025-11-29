#include "KineticBlockEntity.hpp"
#include "Create.hpp"
#include "content/kinetics/base/GeneratingKineticBlockEntity.hpp"
#include "content/kinetics/simpleRelays/ICogWheel.hpp"

KineticNetwork *KineticBlockEntity::getOrCreateNetwork()
{
    return Create::TORQUE_PROPAGATOR.getOrCreateNetworkFor(this);
}

void KineticBlockEntity::switchToBlockState(Dimension &world, const BlockPos &pos, const Block &state)
{
    if (world.isClientSide())
        return;

    BlockSource& region = *world.mBlockSource;
    BlockActor* blockEntity = region.getBlockEntity(pos);
    const Block& currentState = region.getBlock(pos);
    bool isKinetic = blockEntity && KineticBlockEntity::isKineticBlockEntity(*blockEntity);

    if (&currentState == &state)
        return;

    if (!blockEntity || !isKinetic) {
        region.setBlock(pos, state, 3, nullptr, nullptr);
        return;
    }

    KineticBlockEntity* kineticBlockEntity = static_cast<KineticBlockEntity*>(blockEntity);
    KineticBlock* kineticBlock = dynamic_cast<KineticBlock*>(state.mLegacyBlock.get());

    if (kineticBlock && !kineticBlock->areStatesKineticallyEquivalent(currentState, state)) {
        if (kineticBlockEntity->hasNetwork())
            kineticBlockEntity->getOrCreateNetwork()->remove(kineticBlockEntity->getShared<KineticBlockEntity>());

        kineticBlockEntity->detachKinetics();
        kineticBlockEntity->removeSource();
    }

    if (auto* generatingBlockEntity = dynamic_cast<GeneratingKineticBlockEntity*>(blockEntity)) {
        generatingBlockEntity->reActivateSource = true;
    }

    region.setBlock(pos, state, 3, nullptr, nullptr);
}

bool KineticBlockEntity::canPropagateDiagonally(IRotate &block, const Block &state)
{
    return ICogWheel::isSmallCog(state);
}