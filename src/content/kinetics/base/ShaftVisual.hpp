#pragma once
#include "content/kinetics/base/SingleAxisRotatingVisual.hpp"
#include "flywheel/api/model/Models.hpp"
#include "AllPartialModels.hpp"

template <typename T>
class ShaftVisual : public SingleAxisRotatingVisual<T> {
public:
    ShaftVisual(VisualizationContext& ctx, T* blockEntity, float partialTick) 
        : SingleAxisRotatingVisual<T>(ctx, blockEntity, partialTick, Models::partial(AllPartialModels::SHAFT)) {}
};