#pragma once
#include "flywheel/api/instance/Instance.hpp"

class AbstractInstance : public Instance {
private:
    std::shared_ptr<InstanceHandle> _handle;
    std::shared_ptr<InstanceTypeBase> _type;

public:
    AbstractInstance(std::shared_ptr<InstanceTypeBase> type, std::shared_ptr<InstanceHandle> handle)
        : _type(type), _handle(handle) {}

    virtual std::shared_ptr<InstanceTypeBase> type() const override final {
        return _type;
    }

    virtual std::shared_ptr<InstanceHandle> handle() const override final {
        return _handle;
    }

    virtual void setChanged() override final {
        _handle->setChanged();
    }

    virtual void deleteInstance() override final {
        _handle->setDeleted();
    }

    virtual void setVisible(bool visible) override final {
        _handle->setVisible(visible);
    }
};