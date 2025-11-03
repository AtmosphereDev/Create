#pragma once
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"

class ShaftRenderer : public KineticBlockEntityRenderer {
public:
    virtual const Block& getRenderedBlockState(const KineticBlockEntity& entity) const override {
        return shaft(getRotationAxisOf(entity));
    }
};