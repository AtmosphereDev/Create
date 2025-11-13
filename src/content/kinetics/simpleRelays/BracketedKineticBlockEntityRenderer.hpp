#pragma once
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"
#include "content/kinetics/simpleRelays/SimpleKineticBlockEntity.hpp"
#include "content/kinetics/base/KineticBlockEntityVisual.hpp"
#include "content/kinetics/simpleRelays/CogWheelBlock.hpp"

class BracketedKineticBlockEntityRenderer : public KineticBlockEntityRenderer {
public:
    mce::TexturePtr mLargeCogwheelTexture;

    virtual void loadTextures(std::shared_ptr<mce::TextureGroup> textures) override {
        KineticBlockEntityRenderer::loadTextures(textures);
        mLargeCogwheelTexture = textures->getTexture("textures/entity/large_cogwheel", false, std::nullopt, cg::TextureSetLayerType::Normal);
    }

    virtual void renderSafe(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) override {
        // Non large cogwheels just pass through to base        
        const SimpleKineticBlockEntity& be = static_cast<const SimpleKineticBlockEntity&>(data.entity);
        if (AllBlocks::LARGE_COGWHEEL != be.getBlock().mLegacyBlock) {
            KineticBlockEntityRenderer::renderSafe(self, ctx, data);
            return;
        }

        Facing::Axis axis = getRotationAxisOf(be);
        FacingID facing = Facing::fromDirectionAndAxis(Facing::AxisDirection::POSITIVE, axis);
        
        renderRotatingBuffer(self, be, ctx, Models::partial(ctx.mScreenContext.tessellator, AllPartialModels::SHAFTLESS_LARGE_COGWHEEL), mLargeCogwheelTexture);
        
        Vec3 renderPos = Vec3(be.getBlockPos()) - ctx.mCameraTargetPosition;
        MatrixStack& stack = ctx.mScreenContext.camera->worldMatrixStack;
        auto mat = stack.push(); // push into local space
        
        float angle = getAngleForLargeCogShaft(be, axis);
        auto model = Models::partial(ctx.mScreenContext.tessellator, AllPartialModels::COGWHEEL_SHAFT);
        mat->translate(0, -0.5f, 0);
        applyModelRotation(be, *mat);
        kineticRotationTransform(be, *mat, axis, angle);
        mat->translate(renderPos.x + 0.5f, renderPos.y + 0.5f, renderPos.z + 0.5f); // uncenter model around 0,0,0
        
        model->render(ctx, self.getStaticEntityMaterial());

        mat.stack->pop();
    };

    virtual std::shared_ptr<Model> getModel(Tessellator& tess, const KineticBlockEntity& be, const Block& state) const override {
        // called when passing through to base when not rendering large cogwheels
        if (AllBlocks::COGWHEEL == state.mLegacyBlock) {
            return Models::partial(tess, AllPartialModels::COGWHEEL);
        }
        
        return Models::partial(tess, AllPartialModels::SHAFT);
    }

    static float getAngleForLargeCogShaft(const SimpleKineticBlockEntity& be, Facing::Axis axis) {
        const BlockPos& pos = be.getBlockPos();
        float offset = getShaftAngleOffset(axis, pos);
        float time = getTime();
        float angle = glm::mod((time * be.getSpeed() * 3.0f / 10.0f + offset), 360.0f) / 180.0f * glm::pi<float>();
        return angle;
    }

    static float getShaftAngleOffset(Facing::Axis axis, const BlockPos& pos) {
        if (KineticBlockEntityVisual<KineticBlockEntity>::shouldOffset(axis, pos)) {
            return 22.5f;
        }
        return 0.0f;
    }
};