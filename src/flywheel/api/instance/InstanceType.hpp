#pragma once
#include <memory>
#include "flywheel/api/instance/InstanceHandle.hpp"

class InstanceTypeBase {
public:
    virtual ~InstanceTypeBase() = default;
};

template <typename I>
class InstanceType : public InstanceTypeBase {
public:
    virtual std::shared_ptr<I> create(std::shared_ptr<InstanceHandle> handle) = 0;
};