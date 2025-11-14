#include "BeltInventory.hpp"
#include "content/kinetics/belt/BeltBlockEntity.hpp"
#include "content/kinetics/belt/BeltBlock.hpp"

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
            insert(std::move(stack));
        }

        toInsert.clear();

        items.erase(
            std::remove_if(
                items.begin(),
                items.end(),
                [this](const TransportedItemStack& stack) {
                    // Check if stack exists in toRemove
                    return std::any_of(toRemove.begin(), toRemove.end(),
                                       [&](const TransportedItemStack& r) { return &r == &stack; });
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
        currentItem = &*iterator;

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
            Log::Info("BeltInventory::tick EJECT not implemented yet");
            // ++iterator;
            // remove iterator
            items.erase(iterator);
            currentItem = nullptr;
            continue;
        }

        ++iterator;
    }
}

void BeltInventory::ejectAll()
{
    Log::Info("BeltInventory::ejectAll not implemented yet");
}
