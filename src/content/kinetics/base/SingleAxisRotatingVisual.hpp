#pragma once
#include "content/kinetics/base/KineticBlockEntityVisual.hpp"

template <typename T> 
requires std::is_base_of_v<KineticBlockEntity, T>
class SingleAxisRotatingVisual : public KineticBlockEntityVisual<T> {
protected:
    std::shared_ptr<RotatingInstance> rotatingModel;

public:
    SingleAxisRotatingVisual(const VisualizationContext& ctx, T* blockEntity, float partialTick, FacingID from, std::shared_ptr<Model> model) 
        : KineticBlockEntityVisual<T>(ctx, blockEntity, partialTick), rotatingModel(nullptr)
    {
        this->instancerProvider().instancer(AllInstanceTypes.ROTATING, model);
    }
};