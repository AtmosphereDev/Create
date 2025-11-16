#include "BeltInventory.hpp"
#include "content/kinetics/belt/BeltBlockEntity.hpp"
#include "content/kinetics/belt/BeltBlock.hpp"
#include "content/kinetics/belt/behaviour/BeltProcessingBehaviour.hpp"
#include "content/kinetics/belt/behaviour/TransportedItemStackHandlerBehaviour.hpp"
#include "content/kinetics/belt/behaviour/DirectBeltInputBehaviour.hpp"
#include "foundation/utility/BlockHelper.hpp"
#include <mc/src/common/world/Facing.hpp>

void BeltInventory::tick()
{
    // Residual item for "smooth" transitions
    if (lazyClientItem.has_value()) {
        if (lazyClientItem->locked) {
            lazyClientItem = std::nullopt;
        }
        else {
            lazyClientItem->locked = true;
        }
    }

    // Added/Removed items from previous cycle
    if (!toInsert.empty() || !toRemove.empty()) {
        for (auto& stack : toInsert) {
            insert(stack);
        }

        toInsert.clear();

        items.erase(
            std::remove_if(
                items.begin(),
                items.end(),
                [this](const std::shared_ptr<TransportedItemStack>& stack) {
                    // Check if stack exists in toRemove
                    return std::any_of(toRemove.begin(), toRemove.end(),
                                       [&](const std::shared_ptr<TransportedItemStack>& r) { return r.get() == stack.get(); });
                }),
            items.end()
        );
        
        toRemove.clear();
        belt->notifyUpdate();
    }

    if (belt->getSpeed() == 0.0f) 
        return;

    // Reverse item collection if belt just reversed
    if (beltMovementPositive != belt->getDirectionAwareBeltMovementSpeed() > 0) {
        beltMovementPositive = !beltMovementPositive;
        std::reverse(items.begin(), items.end());
        belt->notifyUpdate();
    }

    // Assuming the first entry is the furthest along the belt
    TransportedItemStack* stackInFront = nullptr;
    TransportedItemStack* currentItem = nullptr;

    // Usefull stuff
    float beltSpeed = belt->getDirectionAwareBeltMovementSpeed();
    FacingID movementFacing = belt->getMovementFacing();
    bool horizontal = belt->getBlock().getState<BeltSlope::Type>(BeltBlock::SLOPE()) == BeltSlope::HORIZONTAL;
    float spacing = 1.0f;
    Dimension& world = belt->getLevel();
    bool onClient = world.isClientSide() && !belt->isVirtual();

    Ending ending = Ending::UNRESOLVED;

    for (auto iterator = items.begin(); iterator != items.end(); /*purposefully no iterator*/) {
        stackInFront = currentItem;
        currentItem = iterator->get();

        if (currentItem->stack.isNull()) {
            items.erase(iterator);
            currentItem = nullptr;
            continue;
        }

        float movement = beltSpeed;
        if (onClient) {
            // movement *= ServerSpeedProvider.get(); TODO
        }

        // Don't move if held by processing (client)
        if (world.isClientSide() && currentItem->locked) {
            ++iterator;
            continue;
        }

        // Don't move if held by external components
        if (currentItem->lockedExternally) {
            currentItem->lockedExternally = false;
            ++iterator;
            continue;
        }

        // Don't move if other items are waiting in front
        bool noMovement = false;
        float currentPos = currentItem->beltPosition;
        if (stackInFront != nullptr) {
            float diff = stackInFront->beltPosition - currentPos;
            if (std::abs(diff) <= spacing)
                noMovement = true;
            movement = beltMovementPositive ? std::min(movement, diff - spacing) : std::max(movement, diff + spacing);
        }

        // Don't move beyond the edge
        float diffToEnd = beltMovementPositive ? belt->beltLength - currentPos : -currentPos;
        if (std::abs(diffToEnd) <= std::abs(movement) + 1) {
            if (ending == Ending::UNRESOLVED)
                ending = resolveEnding();
            diffToEnd += beltMovementPositive ? -ending.margin : ending.margin;
        }

        float limitedMovement = beltMovementPositive ? std::min(movement, diffToEnd) : std::max(movement, diffToEnd);
        float nextOffset = currentItem->beltPosition + limitedMovement;

        // Belt item processing
        if (!onClient && horizontal) {
            ItemStack& item = currentItem->stack;
            if (handleBeltProcessingAndCheckIfRemoved(*currentItem, nextOffset, noMovement)) {
                iterator = items.erase(iterator);
                currentItem = nullptr;
                belt->notifyUpdate();
                continue;
            }
            if (item != currentItem->stack) 
                belt->notifyUpdate();
            if (currentItem->locked) {
                ++iterator;
                continue;
            }
        }

        // Belt funnels
        // if (BeltFunnelInteractionHandler.checkForFunnels(this, currentItem, nextOffset)) {
        //     ++iterator;
        //     continue;
        // }

        if (noMovement) {
            ++iterator;
            continue;
        }

        // Belt tunnels
        // if (BeltTunnelInteractionHandler.flapTunnelsAndCheckIfStuck(this, currentItem, nextOffset)) {
        //     ++iterator;
        //     continue;
        // }

        // Horizontal Crushing Wheels
        // if (BeltCrusherInteractionHandler.checkForCrushers(this, currentItem, nextOffset)) {
        //     ++iterator;
        //     continue;
        // }

        // Apply movement
        currentItem->beltPosition += limitedMovement;
        float diffToMiddle = currentItem->getTargetSideOffset() - currentItem->sideOffset;
        currentItem->sideOffset += std::clamp(diffToMiddle * std::abs(limitedMovement) * 6.0f, -std::abs(diffToMiddle), std::abs(diffToMiddle));
        currentPos = currentItem->beltPosition;

        // Movement successfull
        if (limitedMovement == movement || onClient) {
            ++iterator;
            continue;
        }

        // End reached
        int lastOffset = beltMovementPositive ? belt->beltLength - 1 : 0;
        BlockPos nextPosition = BeltHelper::getPositionForOffset(belt, beltMovementPositive ? belt->beltLength : -1);

        if (ending == Ending::FUNNEL) {
            ++iterator;
            continue;
        }

        if (ending == Ending::INSERT) {
            Log::Info("BeltInventory::tick INSERT not implemented yet");
            ++iterator;
            continue;
        }

        if (ending == Ending::BLOCKED) {
            ++iterator;
            continue;
        }

        if (ending == Ending::EJECT) {
            eject(*currentItem);
            items.erase(iterator);
            currentItem = nullptr;
            // flapTunnel(this, lastOffset, movementFacing, false);
            belt->notifyUpdate();
            continue;
        }

        ++iterator;
    }
}

bool BeltInventory::handleBeltProcessingAndCheckIfRemoved(TransportedItemStack &currentItem, float nextOffset, bool noMovement)
{
    int currentSegment = static_cast<int>(std::floor(currentItem.beltPosition));

    if (currentItem.locked) {
        auto processingBehaviour = getBeltProcessingAtSegment(currentSegment);
        auto stackHandlerBehaviour = getTransportedItemStackHandlerAtSegment(currentSegment);

        if (stackHandlerBehaviour == nullptr)
            return false;
        if (processingBehaviour == nullptr) {
            currentItem.locked = false;
            belt->notifyUpdate();
            return false;
        }

        auto result = processingBehaviour->handleHeldItem(currentItem, *stackHandlerBehaviour);
        if (result == BeltProcessingBehaviour::ProcessingResult::REMOVE) 
            return true;
        if (result == BeltProcessingBehaviour::ProcessingResult::HOLD)
            return false;
        
        currentItem.locked = false;
        belt->notifyUpdate();
        return false;
    }

    if (noMovement)
        return false;

    // See if any new belt processing catches the item
    if (currentItem.beltPosition > 0.5f || beltMovementPositive) {
        int firstUpcomingSegment = static_cast<int>(std::floor(currentItem.beltPosition + (beltMovementPositive ? 0.5f : -0.5f)));
        int step = beltMovementPositive ? 1 : -1;

        for (int segment = firstUpcomingSegment; beltMovementPositive ? segment + 0.5f <= nextOffset : segment + 0.5f >= nextOffset; segment += step) {
            auto processingBehaviour = getBeltProcessingAtSegment(segment);
            auto stackHandlerBehaviour = getTransportedItemStackHandlerAtSegment(segment);

            if (processingBehaviour == nullptr || stackHandlerBehaviour == nullptr)
                continue;

            if (BeltProcessingBehaviour::isBlocked(belt->getLevel(), BeltHelper::getPositionForOffset(belt, segment)))
                continue;  
            
            auto result = processingBehaviour->handleReceivedItem(currentItem, *stackHandlerBehaviour);
            if (result == BeltProcessingBehaviour::ProcessingResult::REMOVE) 
                return true;
            if (result == BeltProcessingBehaviour::ProcessingResult::HOLD) {
                currentItem.beltPosition = segment + 0.5f + (beltMovementPositive ? 1 / 512.0f : -1 / 512.0f);
                currentItem.locked = true;
                belt->notifyUpdate();
                return false;
            }
        }
    }

    return false;
}

std::shared_ptr<BeltProcessingBehaviour> BeltInventory::getBeltProcessingAtSegment(int segment)
{
    return BlockEntityBehaviour::get<BeltProcessingBehaviour>(*belt->getLevel().mBlockSource.get(), BeltHelper::getPositionForOffset(belt, segment)
        .above(2), BeltProcessingBehaviour::TYPE);
}

std::shared_ptr<TransportedItemStackHandlerBehaviour> BeltInventory::getTransportedItemStackHandlerAtSegment(int segment)
{
    return BlockEntityBehaviour::get<TransportedItemStackHandlerBehaviour>(*belt->getLevel().mBlockSource.get(), BeltHelper::getPositionForOffset(belt, segment)
        .above(2), TransportedItemStackHandlerBehaviour::TYPE);
}

CompoundTag BeltInventory::write() const
{
    CompoundTag nbt = CompoundTag();
    ListTag itemsList = ListTag();

    for (const auto& stack : items) {
        itemsList.add(stack->serializeNBT());
    }

    nbt.put("Items", std::move(itemsList));
    if (lazyClientItem.has_value()) {
        nbt.put("LazyClientItem", lazyClientItem->serializeNBT());
    }
    nbt.putByte("PositiveOrder", beltMovementPositive ? 1 : 0);
    Log::Info("BeltInventory::write completed {}", items.size());
    return nbt;
}

void BeltInventory::read(const CompoundTag &nbt)
{
    items.clear();
    const ListTag* itemsList = nbt.getList("Items");

    for (const auto& inbt : itemsList->mList) {
        const CompoundTag& tag = static_cast<const CompoundTag&>(*inbt);
        TransportedItemStack stack = TransportedItemStack::deserializeNBT(tag);
        Log::Info("BeltInventory::read loading item {} at position {}", stack.stack.mItem->mFullName, stack.beltPosition);
        items.push_back(std::make_shared<TransportedItemStack>(stack));
    }

    if (nbt.contains("LazyClientItem", Tag::Type::Compound)) {
        const CompoundTag& lazyTag = *nbt.getCompound("LazyClientItem");
        TransportedItemStack lazyStack = TransportedItemStack::deserializeNBT(lazyTag);
        lazyClientItem = lazyStack;
    }

    beltMovementPositive = nbt.getByte("PositiveOrder") != 0;
}

void BeltInventory::eject(TransportedItemStack &stack)
{
    const ItemStack& ejected = stack.stack;
    Vec3 outPos = BeltHelper::getVectorForOffset(belt, stack.beltPosition);
    float movementSpeed = std::max(std::abs(belt->getBeltMovementSpeed()), 1 / 8.0f);
    Vec3 outMotion = (Vec3::atLowerCornerOf(belt->getBeltChainDirection()) * movementSpeed) + Vec3(0, 1 / 8.0f, 0);
    outPos = outPos + outMotion.normalized() * 0.001;
    Log::Info("Ejecting item {} at position ({}, {}, {}) with motion ({}, {}, {})", ejected.mItem->mFullName, outPos.x, outPos.y, outPos.z, outMotion.x, outMotion.y, outMotion.z);
}

void BeltInventory::ejectAll()
{
    Log::Info("BeltInventory::ejectAll not implemented yet");
}

BeltInventory::Ending BeltInventory::resolveEnding()
{
    BlockSource& region = *belt->getLevel().mBlockSource;
    BlockPos nextPosition = BeltHelper::getPositionForOffset(belt, beltMovementPositive ? belt->beltLength : -1);

    auto inputBehaviour = BlockEntityBehaviour::get<DirectBeltInputBehaviour>(region, nextPosition, DirectBeltInputBehaviour::TYPE);
    if (inputBehaviour != nullptr) 
        return BeltInventory::Ending::INSERT;
    
    if (BlockHelper::hasBlockSolidSide(region.getBlock(nextPosition), region, nextPosition, Facing::getOpposite(belt->getMovementFacing())))
        return BeltInventory::Ending::BLOCKED;

    return BeltInventory::Ending::EJECT;
}

bool BeltInventory::canInsertAt(int segment)
{
    return canInsertAtFromSide(segment, FacingID::UP);
}

bool BeltInventory::canInsertAtFromSide(int segment, FacingID side)
{
    float segmentPos = (float)segment;
    if (belt->getMovementFacing() == Facing::getOpposite(side))
        return false;
    if (belt->getMovementFacing() != side) 
        segmentPos += 0.5f;
    else if (!beltMovementPositive)
        segmentPos += 1.0f;

    for (const auto& stack : items) {
        if (isBlocking(segment, side, segmentPos, *stack.get()))
            return false;
    }

    for (const auto& stack : toInsert) {
        if (isBlocking(segment, side, segmentPos, *stack.get()))
            return false;
    }

    return true;
}

bool BeltInventory::isBlocking(int segment, FacingID side, float segmentPos, TransportedItemStack &stack)
{
    float currentPos = stack.beltPosition;
    return stack.insertedAt == segment && stack.insertedFrom == side 
        && (beltMovementPositive ? currentPos <= segmentPos + 1 : currentPos >= segmentPos - 1);
}

void BeltInventory::addItem(std::shared_ptr<TransportedItemStack> stack)
{
    toInsert.push_back(stack);
}

void BeltInventory::insert(std::shared_ptr<TransportedItemStack> newStack)
{
    if (items.empty()) {
        items.push_back(std::move(newStack));
        return;
    }

    int index = 0;
    for (auto& stack : items) {
        if (stack->compareTo(*newStack.get()) > 0 == beltMovementPositive) {
            break;
        }
        index++;
    }

    items.insert(items.begin() + index, std::move(newStack));
}

std::shared_ptr<TransportedItemStack> BeltInventory::getStackAtOffset(int offset)
{
    float min = static_cast<float>(offset);
    float max = min + 1.0f;

    for (auto& stack : items) {
        if (stack->beltPosition > max || stack->beltPosition < min)
            continue;
        
        if (toRemove.end() != std::find_if(toRemove.begin(), toRemove.end(),
            [&](const std::shared_ptr<TransportedItemStack>& r) { return r.get() == stack.get(); })) {
            continue;
        }

        return stack;
    }

    return nullptr;
}