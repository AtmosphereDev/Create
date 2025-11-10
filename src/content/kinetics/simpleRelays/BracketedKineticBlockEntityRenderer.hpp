#pragma once
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"
#include "content/kinetics/simpleRelays/SimpleKineticBlockEntity.hpp"
#include "content/kinetics/base/KineticBlockEntityVisual.hpp"

class BracketedKineticBlockEntityRenderer : public KineticBlockEntityRenderer {
public:
    mce::TexturePtr mLargeCogwheelTexture;

    virtual void loadTextures(std::shared_ptr<mce::TextureGroup> textures) override {
        KineticBlockEntityRenderer::loadTextures(textures);
        mLargeCogwheelTexture = textures->getTexture("textures/entity/large_cogwheel", false, std::nullopt, cg::TextureSetLayerType::Normal);
    }

    virtual void renderSafe(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) const override {
        // todo handle non large cogwheels
        // if (!AllBlocks.LARGE_COGWHEEL.has(be.getBlockState())) {
		// 	super.renderSafe(be, partialTicks, ms, buffer, light, overlay);
		// 	return;
		// }

        const SimpleKineticBlockEntity& be = static_cast<const SimpleKineticBlockEntity&>(data.entity);
        Vec3 renderPos = Vec3(be.getBlockPos()) - ctx.mCameraTargetPosition;
        MatrixStack& stack = ctx.mScreenContext.camera->worldMatrixStack;
        auto worldSpace = stack.push(); // push into local space
        worldSpace->translate(0, 0.5f, 0);

        Facing::Axis axis = getRotationAxisOf(be);
        FacingID facing = Facing::fromDirectionAndAxis(Facing::AxisDirection::POSITIVE, axis);

        renderRotatingBuffer(self, be, ctx, Models::partial(ctx.mScreenContext.tessellator, AllPartialModels::SHAFTLESS_LARGE_COGWHEEL), mLargeCogwheelTexture);

        float angle = getAngleForLargeCogShaft(be, axis);
        auto model = Models::partial(ctx.mScreenContext.tessellator, AllPartialModels::COGWHEEL_SHAFT);
        applyModelRotation(be, *worldSpace);
        kineticRotationTransform(be, *worldSpace, axis, angle);
        worldSpace->translate(renderPos.x + 0.5f, renderPos.y - 0.5f, renderPos.z + 0.5f); // uncenter model around 0,0,0
        
        for (const auto& mesh : model->meshes) {
            mesh.mesh.renderMesh(ctx.mScreenContext, self.getStaticEntityMaterial(), mShaftTexture);
        }

        worldSpace.stack->pop();
    };

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