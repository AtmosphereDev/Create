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
        std::vector<std::shared_ptr<TransportedItemStack>> outputs;
        std::shared_ptr<TransportedItemStack> heldOutput;

        static TransportedResult doNothing() {
            return TransportedResult({}, nullptr);
        }

        static TransportedResult removeItem() {
            return TransportedResult({}, nullptr);
        }

        static TransportedResult convertTo(const TransportedItemStack& output) {
            return TransportedResult({ std::make_shared<TransportedItemStack>(output) }, nullptr);
        }

        static TransportedResult convertTo(const std::vector<TransportedItemStack>& outputs);

        static TransportedResult convertToAndLeaveHeld(const std::vector<TransportedItemStack>& outputs,
                                                       const TransportedItemStack& heldOutput);

        bool doesNothing() const {
            return outputs.size() == 0;
        }

        bool didntChangeFrom(const ItemStack& stackBefore) const;

        const std::vector<std::shared_ptr<TransportedItemStack>>& getOutputs() const {
            return outputs;
        }

        bool hasHeldOutput() const {
            return heldOutput != nullptr;
        }

        std::shared_ptr<TransportedItemStack> getHeldOutput() const {
            return heldOutput;
        }

    private:
        TransportedResult(std::vector<std::shared_ptr<TransportedItemStack>> outputs,
                          std::shared_ptr<TransportedItemStack> heldOutput)
            : outputs(std::move(outputs)), heldOutput(std::move(heldOutput)) {}
    };

    using ProcessingCallback =
        std::function<void(float, std::function<std::optional<TransportedResult>(TransportedItemStack&)>)>;

    using PositionGetter =
        std::function<Vec3(TransportedItemStack&)>;

private:
    ProcessingCallback processingCallback;
    PositionGetter positionGetter;

public:
    explicit TransportedItemStackHandlerBehaviour(std::shared_ptr<SmartBlockEntity> be, ProcessingCallback callback)
        : BlockEntityBehaviour(be),
          processingCallback(std::move(callback)),
          positionGetter([be](TransportedItemStack&) {
            return BlockPos(be->getBlockPos()).center();
          }) {}

    TransportedItemStackHandlerBehaviour& withStackPlacement(PositionGetter func) {
        positionGetter = std::move(func);
        return *this;
    }

    void handleProcessingOnAllItems(std::function<std::optional<TransportedResult>(TransportedItemStack&)> processFunction) {
        handleCenteredProcessingOnAllItems(0.51f, std::move(processFunction));
    }

    void handleProcessingOnItem(TransportedItemStack& item, const TransportedResult& processOutput) {
        handleCenteredProcessingOnAllItems(0.51f, [&](TransportedItemStack& t) -> std::optional<TransportedResult> {
            if (&t == &item)
                return processOutput;
            return std::nullopt;
        });
    }

    void handleCenteredProcessingOnAllItems(float maxDistanceFromCenter,
                                            std::function<std::optional<TransportedResult>(TransportedItemStack&)> processFunction) {
        this->processingCallback(
            maxDistanceFromCenter,
            std::move(processFunction)
        );
    }

    Vec3 getWorldPositionOf(TransportedItemStack& transported) {
        return positionGetter(transported);
    }

    const BehaviourType& getType() const override {
        return TYPE;
    }
};
