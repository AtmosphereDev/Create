#include "BeltBlockEntity.hpp"
#include "BeltBlock.hpp"
#include "AllBlocks.hpp"
#include "content/kinetics/belt/transport/BeltInventory.hpp"
#include "content/kinetics/belt/transport/ItemHandlerBeltSegment.hpp"
#include "mc/src/common/SharedPtr.hpp"

void BeltBlockEntity::registerCapabilities(RegisterCapabilitiesEvent &event)
{
    // Dereference the WeakPtr and cast to const BlockLegacy*
    const BlockLegacy* beltBlock = static_cast<const BlockLegacy*>(AllBlocks::BELT.get());

    // Reference to the BlockCapability object
    auto& blockCapability = event.capabilities.itemHandler.BLOCK;

    // Provider lambda
    BlockCapability<IItemHandler, std::optional<FacingID>>::ProviderFn provider = [](Level& level, const BlockPos& pos, const Block& state, BlockActor* be, const std::optional<FacingID>& side) -> IItemHandler* {
        
        if (!BeltBlock::canTransportObjects(state))
            return nullptr;

        BeltBlockEntity* beltBE = dynamic_cast<BeltBlockEntity*>(be);
        if (beltBE != nullptr && beltBE->itemHandler == nullptr)
            beltBE->initializeItemHandler();

        return beltBE->itemHandler.get();
    };

    event.registerBlockEntity(blockCapability, beltBlock, provider);
}

BeltBlockEntity *BeltBlockEntity::getControllerBE() const
{
    if (!controller.has_value())
        return nullptr;

    BlockActor* actor = level->getBlockSource().getBlockEntity(controller.value());
    return dynamic_cast<BeltBlockEntity*>(actor);
}

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

    initializeItemHandler();

    if (!isController())
        return;

    getInventory()->tick();

    // Todo impl Move entities
}

void BeltBlockEntity::initializeItemHandler()
{
    if (level->isClientSide() || itemHandler != nullptr)
        return;
    if (beltLength == 0 || controller == std::nullopt) 
        return;

    BlockActor* actor = level->getBlockSource().getBlockEntity(controller.value());
    BeltBlockEntity* be = dynamic_cast<BeltBlockEntity*>(actor);
    if (be == nullptr)
        return;

    auto inventory = be->getInventory();
    if (inventory == nullptr)
        return;

    itemHandler = std::make_shared<ItemHandlerBeltSegment>(inventory, index);
    invalidateCapabilities();
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

void BeltBlockEntity::write(CompoundTag &compound, BlockSource &region)
{
    if (controller.has_value()) {
        compound.put("Controller", NbtUtils::writeBlockPos(controller.value()));
    }
    compound.putByte("IsController", isController() ? 1 : 0);
    compound.putInt("Length", beltLength);
    compound.putInt("Index", index);
    // casing
    // covered
    // color

    if (isController()) {
        compound.put("Inventory", getInventory()->write());
    }

    KineticBlockEntity::write(compound, region);
}

void BeltBlockEntity::read(const CompoundTag &compound, BlockSource &region)
{
    KineticBlockEntity::read(compound, region);

    if (compound.getByte("IsController") != 0) {
        controller = mPosition;
    }

    // color

    if (!wasMoved) {
        if (!isController()) {
            controller = NbtUtils::readBlockPos(*compound.getCompound("Controller"));
        }
        // trackerUpdateTag
        index = compound.getInt("Index");
        beltLength = compound.getInt("Length");
    }

    if (isController())
        getInventory()->read(*compound.getCompound("Inventory"));

    // CasingType casingBefore = casing;
    // boolean coverBefore = covered;
    // casing = NBTHelper.readEnum(compound, "Casing", CasingType.class);
    // covered = compound.getBoolean("Covered");

    if (!region.isClientSide())
        return;

    // if (casingBefore == casing && coverBefore == covered)
    //     return;
    // if (!isVirtual())
    //     requestModelDataUpdate();
    // if (hasLevel())
    //     level.sendBlockUpdated(getBlockPos(), getBlockState(), getBlockState(), 16);
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

BlockPos BeltBlockEntity::getBeltChainDirection() const
{
    return getMovementDirection(true, true);
}

BlockPos BeltBlockEntity::getMovementDirection(bool firstHalf, bool ignoreHalves) const
{
    if (getSpeed() == 0)
        return BlockPos::ZERO;

    const Block& blockState = getBlock();
    FacingID beltFacing = blockState.getState<FacingID>(HorizontalKineticBlock::HORIZONTAL_FACING());
    BeltSlope::Type slope = blockState.getState<BeltSlope::Type>(BeltBlock::SLOPE());
    BeltPart::Type part = blockState.getState<BeltPart::Type>(BeltBlock::PART());
    Facing::Axis axis = Facing::getAxis(beltFacing);

    FacingID movementFacing = Facing::fromDirectionAndAxis(
        axis == Facing::Axis::X ? Facing::AxisDirection::NEGATIVE : Facing::AxisDirection::POSITIVE, 
        axis
    );
    bool notHorizontal = slope != BeltSlope::HORIZONTAL;
    if (getSpeed() < 0)
        movementFacing = Facing::getOpposite(movementFacing);
    BlockPos movement = Facing::normal(movementFacing);

    bool slopeBeforeHalf = (part == BeltPart::END) == (Facing::getAxisDirection(beltFacing) == Facing::AxisDirection::POSITIVE);
    bool onSlope = notHorizontal && (part == BeltPart::MIDDLE || slopeBeforeHalf == firstHalf || ignoreHalves);
    bool movingUp = onSlope && slope == (movementFacing == beltFacing ? BeltSlope::UPWARD : BeltSlope::DOWNWARD);

    if (!onSlope)
        return movement;

    return BlockPos(movement.x, movingUp ? 1 : -1, movement.z);
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

std::shared_ptr<BeltInventory> BeltBlockEntity::getInventory()
{
    if (!isController()) {
		BeltBlockEntity* controllerBE = getControllerBE();
		if (controllerBE != nullptr) return controllerBE->getInventory();
		return nullptr;
    }

    if (inventory == nullptr) {
        inventory = std::make_shared<BeltInventory>(this);
	}
	return inventory;
}

void BeltBlockEntity::applyToAllItems(float maxDistanceFromCenter, const std::function<std::optional<TransportedItemStackHandlerBehaviour::TransportedResult>(TransportedItemStack &)> &func)
{
    BeltBlockEntity* controller = getControllerBE();
    if (controller == nullptr)
        return;
    auto inventory = controller->getInventory();
    if (inventory == nullptr)
        return; 
    inventory->applyToEachWithin(index + 0.5f, maxDistanceFromCenter, func);
}

Vec3 BeltBlockEntity::getWorldPositionOf(const TransportedItemStack &stack) const
{
    BeltBlockEntity* controller = getControllerBE();
    if (controller == nullptr)
        return Vec3::ZERO;

    return BeltHelper::getVectorForOffset(controller, stack.beltPosition);
}
