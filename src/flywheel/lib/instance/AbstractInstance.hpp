#pragma once
#include "flywheel/api/instance/Instance.hpp"

class AbstractInstance : public Instance {
public:
    virtual void setChanged() override final {

    }

    virtual void deleteInstance() override final {

    }

    virtual void setVisible(bool visible) override final {
        
    }
};