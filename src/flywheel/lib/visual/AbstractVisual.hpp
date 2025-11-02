#pragma once
#include "flywheel/api/visual/Visual.hpp"
#include "flywheel/api/visualization/VisualizationContext.hpp"

class VisualizationContext;
class Dimension;
class BlockPos;
class InstancerProvider;

class AbstractVisual : public Visual {
protected:
    VisualizationContext& visualizationContext;
    const Dimension& dimension;
    bool deleted = false;

public:
    AbstractVisual(VisualizationContext& ctx, const Dimension& dimension, float partialTick)
        : visualizationContext(ctx), dimension(dimension) {}

protected:
    virtual void update(float partialTick) override {}

    virtual void _delete() = 0;

    const InstancerProvider& instancerProvider() const {
        return visualizationContext.instancerProvider();
    }

    InstancerProvider& instancerProvider() {
        return visualizationContext.instancerProvider();
    }

    const BlockPos& renderOrigin() const {
        return visualizationContext.renderOrigin();
    }

public:
    virtual void deleteVisual() final override {
        if (deleted) return;
        _delete();
        deleted = true;
    }
};