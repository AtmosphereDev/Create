#pragma once
#include "amethyst/game/capabilities/IItemHandler.hpp"
#include "foundation/item/ItemHelper.hpp"
#include <algorithm>

class ItemHandlerBeltSegment : public IItemHandler {
public:
    std::shared_ptr<BeltInventory> beltInventory;
    int offset;

    ItemHandlerBeltSegment(std::shared_ptr<BeltInventory> beltInventory, int offset)
        : beltInventory(beltInventory), offset(offset) {}

    virtual int getSlots() const override {
        return 1;
    }

    virtual const ItemStack& getStackInSlot(int slot) const override {
        auto stackAtOffset = beltInventory->getStackAtOffset(offset);
        if (stackAtOffset == nullptr) {
            return ItemStack::EMPTY_ITEM;
        }
        return stackAtOffset->stack;
    }

    virtual ItemStack insertItem(int slot, ItemStack& stack, bool simulate) override {
        if (beltInventory->canInsertAt(offset)) {
            ItemStack remainder = ItemHelper::limitCountToMaxStackSize(stack, simulate);
            if (!simulate) {
                auto newStack = std::make_shared<TransportedItemStack>(stack);
                newStack->insertedAt = offset;
                newStack->beltPosition = offset + 0.5f + (beltInventory->beltMovementPositive ? -1 : 1) / 16.0f;
                newStack->prevBeltPosition = newStack->beltPosition;
                beltInventory->addItem(newStack);
                beltInventory->belt->setChanged();
                beltInventory->belt->sendData();
            }
            return remainder;
        }
        return stack;
    }

    virtual ItemStack extractItem(int slot, uint8_t amount, bool simulate) override {
        auto transported = beltInventory->getStackAtOffset(offset);
        if (transported == nullptr) {
            return ItemStack::EMPTY_ITEM;
        }

        amount = std::min(amount, transported->stack.mCount);
        ItemStack extracted = simulate ? transported->stack.copy()
            .split(amount) : transported->stack.split(amount);
        if (!simulate) {
            if (transported->stack.isEmpty()) {
                beltInventory->toRemove.push_back(transported);
            }
            else {
                beltInventory->belt->notifyUpdate();
            }
        }
        return extracted;
    }

    virtual int getSlotLimit(int slot) const override {
        return std::min(getStackInSlot(slot).getMaxStackSize(), uint8_t{64});
    }

    virtual bool isItemValid(int slot, const ItemStack& stack) const override {
        return true;
    }
};