#pragma once
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"

class TransportedItemStack;

class DirectBeltInputBehaviour : public BlockEntityBehaviour {
public:
    using InsertionCallback = std::function<ItemStack(const TransportedItemStack&, FacingID, bool)>;
    using OccupiedPredicate = std::function<bool(FacingID)>;
    using AvailabilityPredicate = std::function<bool(FacingID)>;

    static const BehaviourType TYPE;

private:
    InsertionCallback tryInsert;
    OccupiedPredicate isOccupiedFn;
    AvailabilityPredicate canInsertFn;
    std::function<bool()> supportsBeltFunnels;

    ItemStack defaultInsertionCallback(const TransportedItemStack& inserted, FacingID side, bool simulate);

public:
    explicit DirectBeltInputBehaviour(SmartBlockEntity* be);

    DirectBeltInputBehaviour& allowingBeltFunnelsWhen(std::function<bool()> pred);
    DirectBeltInputBehaviour& allowingBeltFunnels();
    DirectBeltInputBehaviour& onlyInsertWhen(AvailabilityPredicate pred);
    DirectBeltInputBehaviour& considerOccupiedWhen(OccupiedPredicate pred);
    DirectBeltInputBehaviour& setInsertionHandler(InsertionCallback callback);

    bool canInsertFromSide(FacingID side) const;
    bool isOccupied(FacingID side) const;

    ItemStack handleInsertion(const ItemStack& stack, FacingID side, bool simulate);
    ItemStack handleInsertion(const TransportedItemStack& tstack, FacingID side, bool simulate);

    virtual const BehaviourType& getType() const override;

    std::optional<ItemStack> tryExportingToBeltFunnel(const ItemStack& stack, std::optional<FacingID> side, bool simulate);

    bool canSupportBeltFunnels() const;
};