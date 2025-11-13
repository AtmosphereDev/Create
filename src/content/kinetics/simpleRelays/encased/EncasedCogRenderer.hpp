#pragma once
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"
#include "AllPartialModels.hpp"
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include "flywheel/api/model/Models.hpp"
#include "content/kinetics/simpleRelays/BracketedKineticBlockEntityRenderer.hpp"

class EncasedCogRenderer : public KineticBlockEntityRenderer {
private:
    bool large;

public:
    EncasedCogRenderer(bool large) : KineticBlockEntityRenderer() {
        this->large = large;
    }

    virtual void renderSafe(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) override {
        // Large cogs sometimes have to offset their teeth by 11.25 degrees in order to
		// mesh properly
        // KineticBlockEntity& be = static_cast<KineticBlockEntity&>(data.entity);
        // const Block& state = data.block;
        // const IRotate* def = IRotate::asIRotate(state.mLegacyBlock.get());
        // if (!def) return;
        
        // Vec3 renderPos = Vec3(be.getBlockPos()) - ctx.mCameraTargetPosition;
        // MatrixStack& stack = ctx.mScreenContext.camera->worldMatrixStack;
        // auto worldSpace = stack.push(); // push into local space
        // worldSpace->translate(renderPos.x + 0.5f, renderPos.y + 0.5f, renderPos.z + 0.5f);

        // Facing::Axis axis = getRotationAxisOf(be);
        // const BlockPos& pos = be.getBlockPos();
        // float angle = large ? BracketedKineticBlockEntityRenderer::getAngleForLargeCogShaft(be, axis)
        //     : getAngleForBe(be, pos, axis);

        // for (FacingID d : Facing::directionsInAxis(axis)) {
        //     if (!def->hasShaftTowards(be.getLevel(), be.getBlockPos(), state, d))
        //         continue;

            
        // }

        // stack.pop();

        // i started implementing this but i didnt need this yet
    }

    virtual std::shared_ptr<Model> getModel(Tessellator& tess, const KineticBlockEntity& be, const Block& state) const override {
        return Models::partial(tess, large ? AllPartialModels::SHAFTLESS_LARGE_COGWHEEL : AllPartialModels::SHAFTLESS_COGWHEEL);
    }

    virtual void applyModelRotation(const KineticBlockEntity& be, Matrix& mat) const override {
        // CachedBuffers.partialFacingVertical(AllPartialModels.COGWHEEL_SHAFT, be.getBlockState(), facing);
        rotateToFace(
            mat,
            Facing::Name::UP
        );

        rotateToFace(
            mat, 
            Facing::fromDirectionAndAxis(Facing::AxisDirection::POSITIVE, be.getBlock().getState<Facing::Axis>(VanillaStates::PillarAxis))
        );
    }
};