#include "BeltBlockEntity.hpp"
#include "BeltBlock.hpp"
#include "AllBlocks.hpp"
#include "content/kinetics/belt/transport/BeltInventory.hpp"

void BeltBlockEntity::tick(BlockSource &source)
 {
    // jank fix
    if (level == nullptr) return;

    if (beltLength == 0) {
        BeltBlock::initBelt(source, mPosition);
    }

    KineticBlockEntity::tick(source);

    if (AllBlocks::BELT != getBlock().mLegacyBlock) 
        return;

    // initializeItemHandler();

    if (!isController())
        return;

    getInventory()->tick();

    // Todo impl Move entities
}

void BeltBlockEntity::destroy()
{
    KineticBlockEntity::destroy();
    if (isController()) {
        getInventory()->ejectAll();
    }
}

void BeltBlockEntity::invalidate()
{
    KineticBlockEntity::invalidate();
    invalidateCapabilities();
}

float BeltBlockEntity::getDirectionAwareBeltMovementSpeed() const
{
    FacingID facing = getBeltFacing();
    int offset = Facing::getStep(Facing::getAxisDirection(facing));
    if (Facing::getAxis(facing) == Facing::Axis::X) 
        offset *= -1;
    return getBeltMovementSpeed() * offset;
}

bool BeltBlockEntity::hasPulley() const
{
    if (AllBlocks::BELT != getBlock().mLegacyBlock) 
        return false;
    return getBlock().getState<BeltPart::Type>(BeltBlock::PART()) != BeltPart::MIDDLE;
}

bool BeltBlockEntity::canPropagateDiagonally(IRotate &block, const Block &state)
{
    if (!state.hasState(BeltBlock::SLOPE()))
        return false;

    BeltSlope::Type slope = state.getState<BeltSlope::Type>(BeltBlock::SLOPE());
    if (slope == BeltSlope::UPWARD || slope == BeltSlope::DOWNWARD)
        return true;

    return false;
}

float BeltBlockEntity::propagateRotationTo(KineticBlockEntity &target, const Block &stateFrom, const Block &stateTo, const BlockPos &diff, bool connectedViaAxes, bool connectedViaCogs)
{
    BeltBlockEntity* targetBeltBE = dynamic_cast<BeltBlockEntity*>(&target);
    if (!targetBeltBE || connectedViaAxes) return 0;
    return getController() == targetBeltBE->getController() ? 1.0f : 0.0f;
}

FacingID BeltBlockEntity::getMovementFacing() const
{
    Facing::Axis axis = Facing::getAxis(getBeltFacing());
    return Facing::fromDirectionAndAxis(
        getBeltMovementSpeed() < 0 ^ axis == Facing::Axis::X ? Facing::AxisDirection::NEGATIVE : Facing::AxisDirection::POSITIVE, 
        axis
    );
}

FacingID BeltBlockEntity::getBeltFacing() const
{
    return getBlock().getState<FacingID>(HorizontalKineticBlock::HORIZONTAL_FACING());
}

std::optional<BeltInventory> &BeltBlockEntity::getInventory()
{
    return inventory;
}
