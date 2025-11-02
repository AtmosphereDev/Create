#pragma once
#include <concepts>
#include <memory>
#include <functional>
#include "flywheel/api/instance/Instance.hpp"
#include "flywheel/api/instance/InstanceType.hpp"
#include "flywheel/api/instance/InstanceHandle.hpp"

template <typename I>
requires std::is_base_of_v<Instance, I>
class SimpleInstanceType : public InstanceType<I>, public std::enable_shared_from_this<SimpleInstanceType<I>> {
public:
    using FactoryFunc = std::function<std::shared_ptr<I>(
        std::shared_ptr<InstanceType<I>>,
        std::shared_ptr<InstanceHandle>
    )>;

    class Builder {
    private:
        FactoryFunc factory_;

    public:
        explicit Builder(FactoryFunc factory) : factory_(std::move(factory)) {}

        std::shared_ptr<SimpleInstanceType<I>> build() {
            return std::make_shared<SimpleInstanceType<I>>(factory_);
        }
    };

private:
    FactoryFunc factory_;

public:
    explicit SimpleInstanceType(FactoryFunc factory) : factory_(std::move(factory)) {}

    std::shared_ptr<I> create(std::shared_ptr<InstanceHandle> handle) override {
        return factory_(this->shared_from_this(), handle);
    }

    static Builder builder(FactoryFunc factory) {
        return Builder(std::move(factory));
    }
};