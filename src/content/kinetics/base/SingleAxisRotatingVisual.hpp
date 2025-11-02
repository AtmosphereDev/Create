#pragma once
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include "content/kinetics/base/KineticBlockEntityVisual.hpp"
#include "content/kinetics/base/RotatingInstance.hpp"
#include "flywheel/api/model/Model.hpp"
#include "foundation/render/AllInstanceTypes.hpp"

template <typename T> 
requires std::is_base_of_v<KineticBlockEntity, T>
class SingleAxisRotatingVisual : public KineticBlockEntityVisual<T> {
protected:
    std::shared_ptr<RotatingInstance> rotatingModel;

public:
    SingleAxisRotatingVisual(VisualizationContext& ctx, T* blockEntity, float partialTick, FacingID from, std::shared_ptr<Model> model) 
        : KineticBlockEntityVisual<T>(ctx, blockEntity, partialTick), rotatingModel(nullptr)
    {
        rotatingModel = AbstractVisual::instancerProvider().template instancer<RotatingInstance>(AllInstanceTypes::ROTATING, model, 0)
            ->createInstance();

        rotatingModel
            ->rotateToFace(from)
            .setup(blockEntity)
            .setPosition(this->getVisualPosition());

        rotatingModel->setChanged();
    }

    SingleAxisRotatingVisual(VisualizationContext& ctx, T* blockEntity, float partialTick, std::shared_ptr<Model> model) 
        : SingleAxisRotatingVisual(ctx, blockEntity, partialTick, FacingID::UP, model) {}

protected:
    void _delete() override {
        rotatingModel->deleteInstance();
    }
};