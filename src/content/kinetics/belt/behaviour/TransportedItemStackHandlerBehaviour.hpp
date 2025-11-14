#pragma once
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"
#include <functional>
#include <memory>
#include <stdexcept>
#include <vector>
#include "content/kinetics/belt/transport/TransportedItemStack.hpp"

class SmartBlockEntity;
class ItemStack;
class Vec3;
class BlockPos;
class BlockSource;

class TransportedItemStackHandlerBehaviour : public BlockEntityBehaviour {
public:
    static const BehaviourType TYPE;

    class TransportedResult {
    public:
        std::shared_ptr<std::vector<TransportedItemStack>> outputs;
        std::shared_ptr<TransportedItemStack> heldOutput;

        static TransportedResult doNothing() {
            return TransportedResult(nullptr, nullptr);
        }

        static TransportedResult removeItem() {
            return TransportedResult(std::make_shared<std::vector<TransportedItemStack>>(), nullptr);
        }

        static TransportedResult convertTo(const TransportedItemStack& output) {
            auto vec = std::make_shared<std::vector<TransportedItemStack>>();
            vec->push_back(output);
            return TransportedResult(vec, nullptr);
        }

        static TransportedResult convertTo(const std::vector<TransportedItemStack>& outputs);

        static TransportedResult convertToAndLeaveHeld(const std::vector<TransportedItemStack>& outputs,
                                                       const TransportedItemStack& heldOutput);

        bool doesNothing() const {
            return outputs == nullptr;
        }

        bool didntChangeFrom(const ItemStack& stackBefore) const;

        const std::vector<TransportedItemStack>& getOutputs() const {
            if (!outputs)
                throw std::runtime_error("Do not call getOutputs() on a Result that doesNothing().");
            return *outputs;
        }

        bool hasHeldOutput() const {
            return heldOutput != nullptr;
        }

        const TransportedItemStack& getHeldOutput() const {
            if (!heldOutput)
                throw std::runtime_error("Do not call getHeldOutput() on a Result with hasHeldOutput() == false.");
            return *heldOutput;
        }

    private:
        TransportedResult(std::shared_ptr<std::vector<TransportedItemStack>> outputs,
                          std::shared_ptr<TransportedItemStack> heldOutput)
            : outputs(std::move(outputs)), heldOutput(std::move(heldOutput)) {}
    };

    using ProcessingCallback =
        std::function<void(float, std::function<TransportedResult(TransportedItemStack&)>)>;

    using PositionGetter =
        std::function<Vec3(TransportedItemStack&)>;

private:
    ProcessingCallback processingCallback;
    PositionGetter positionGetter;

public:
    explicit TransportedItemStackHandlerBehaviour(SmartBlockEntity* be, ProcessingCallback callback)
        : BlockEntityBehaviour(be),
          processingCallback(std::move(callback)),
          positionGetter([be](TransportedItemStack&) {
            return BlockPos(be->getBlockPos()).center();
          }) {}

    TransportedItemStackHandlerBehaviour& withStackPlacement(PositionGetter func) {
        positionGetter = std::move(func);
        return *this;
    }

    void handleProcessingOnAllItems(std::function<TransportedResult(TransportedItemStack&)> processFunction) {
        handleCenteredProcessingOnAllItems(0.51f, std::move(processFunction));
    }

    void handleProcessingOnItem(TransportedItemStack& item, const TransportedResult& processOutput) {
        handleCenteredProcessingOnAllItems(0.51f, [&](TransportedItemStack& t) -> TransportedResult {
            if (&t == &item)
                return processOutput;
            return TransportedResult::doNothing();
        });
    }

    void handleCenteredProcessingOnAllItems(float maxDistanceFromCenter,
                                            std::function<TransportedResult(TransportedItemStack&)> processFunction) {
        processingCallback(maxDistanceFromCenter, std::move(processFunction));
    }

    Vec3 getWorldPositionOf(TransportedItemStack& transported) {
        return positionGetter(transported);
    }

    const BehaviourType& getType() const override {
        return TYPE;
    }
};
