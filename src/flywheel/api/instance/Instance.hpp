#pragma once
#include <memory>
#include "flywheel/api/instance/InstanceHandle.hpp"

class Instance {
public:
    virtual ~Instance() = default;

    virtual std::shared_ptr<InstanceTypeBase> type() const = 0;

    virtual std::shared_ptr<InstanceHandle> handle() const = 0; 

    virtual void setChanged() {
        handle()->setChanged();
    };

    virtual void deleteInstance() {
        handle()->setDeleted();
    };

    virtual void setVisible(bool visible) {
        handle()->setVisible(visible);
    };
};