#pragma once
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"
#include "flywheel/api/model/Models.hpp"
#include "AllPartialModels.hpp"

class CreativeMotorRenderer : public KineticBlockEntityRenderer {
public:
    CreativeMotorRenderer() : KineticBlockEntityRenderer() {}

    virtual std::shared_ptr<Model> getModel(Tessellator& tess, const KineticBlockEntity& be, const Block& state) const override {
        return Models::partial(tess, AllPartialModels::SHAFT_HALF);
    }

    virtual void applyModelRotation(const KineticBlockEntity& be, Matrix& mat) const override {
        rotateToFace(mat, be.getBlock().getState<FacingID>(VanillaStates::FacingDirection));
    }
};