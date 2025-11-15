#pragma once
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"

class TransportedItemStack;
class TransportedItemStackHandlerBehaviour;
class Dimension;

class BeltProcessingBehaviour : public BlockEntityBehaviour {
public:
    static const BehaviourType TYPE;

    enum class ProcessingResult {
        PASS,
        HOLD,
        REMOVE
    };

    using ProcessingCallback =
        std::function<ProcessingResult(TransportedItemStack&, TransportedItemStackHandlerBehaviour&)>;

private:
    ProcessingCallback onItemEnter;
    ProcessingCallback continueProcessing;

public:
    explicit BeltProcessingBehaviour(SmartBlockEntity* be)
        : BlockEntityBehaviour(be),
          onItemEnter([](auto&, auto&) { return ProcessingResult::PASS; }),
          continueProcessing([](auto&, auto&) { return ProcessingResult::PASS; }) {}

    BeltProcessingBehaviour& whenItemEnters(ProcessingCallback callback) {
        onItemEnter = std::move(callback);
        return *this;
    }

    BeltProcessingBehaviour& whileItemHeld(ProcessingCallback callback) {
        continueProcessing = std::move(callback);
        return *this;
    }

    ProcessingResult handleReceivedItem(TransportedItemStack& stack,
                                        TransportedItemStackHandlerBehaviour& inventory) {
        return onItemEnter(stack, inventory);
    }

    ProcessingResult handleHeldItem(TransportedItemStack& stack,
                                    TransportedItemStackHandlerBehaviour& inventory) {
        return continueProcessing(stack, inventory);
    }

    const BehaviourType& getType() const override {
        return TYPE;
    }

    static bool isBlocked(Dimension& world, const BlockPos& processingSpace);
};