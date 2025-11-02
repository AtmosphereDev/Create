#pragma once

class InstanceHandle {
public:
    virtual void setChanged() = 0;
    virtual void setDeleted() = 0;
    virtual void setVisible(bool visible) = 0;
    virtual bool isVisible() const = 0;
};