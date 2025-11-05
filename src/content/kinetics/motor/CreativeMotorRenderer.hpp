#pragma once
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"
#include "flywheel/api/model/Models.hpp"
#include "AllPartialModels.hpp"

class CreativeMotorRenderer : public KineticBlockEntityRenderer {
public:
    mce::TexturePtr mCreativeMotorTexture;

    CreativeMotorRenderer() : KineticBlockEntityRenderer() {}

    virtual void loadTextures(std::shared_ptr<mce::TextureGroup> textures) override {
        KineticBlockEntityRenderer::loadTextures(textures);
        mCreativeMotorTexture = textures->getTexture("textures/entity/creative_motor", false, std::nullopt, cg::TextureSetLayerType::Normal);
    }

    virtual void renderAdditional(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) const override {
        auto model = Models::partial(ctx.mScreenContext.tessellator, AllPartialModels::CREATIVE_MOTOR);
        KineticBlockEntity& be = static_cast<KineticBlockEntity&>(data.entity);
        
        auto& matrixStack = ctx.mScreenContext.camera->worldMatrixStack;
        auto mat = matrixStack.push();

        Log::Info("test");

        applyModelRotation(be, *mat);
        mat->translate(0, -0.5f, 0); // re-align the vertical

        for (const auto& mesh : model->meshes) {
            mesh.mesh.renderMesh(ctx.mScreenContext, self.getStaticEntityMaterial(), mCreativeMotorTexture);
        }

        matrixStack.pop();
    }
 
    // virtual void renderSafe(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) const override {
    //     KineticBlockEntityRenderer::renderSafe(self, ctx, data);

    //     auto model = Models::partial(ctx.mScreenContext.tessellator, AllPartialModels::CREATIVE_MOTOR);
    //     KineticBlockEntity& be = static_cast<KineticBlockEntity&>(data.entity);
        
    //     auto matrixStack = ctx.mScreenContext.camera->worldMatrixStack;
    //     auto mat = matrixStack.push();

    //     applyModelRotation(be, *mat);

    //     for (const auto& mesh : model->meshes) {
    //         mesh.mesh.renderMesh(ctx.mScreenContext, self.getStaticEntityMaterial(), mCreativeMotorTexture);
    //     }

    //     matrixStack.pop();
    // }

    virtual std::shared_ptr<Model> getModel(Tessellator& tess, const KineticBlockEntity& be, const Block& state) const override {
        return Models::partial(tess, AllPartialModels::SHAFT_HALF);
    }

    virtual void applyModelRotation(const KineticBlockEntity& be, Matrix& mat) const override {
        rotateToFace(mat, be.getBlock().getState<FacingID>(VanillaStates::FacingDirection));
    }
};