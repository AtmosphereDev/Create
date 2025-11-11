#pragma once
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"
#include "flywheel/api/model/Models.hpp"
#include "AllPartialModels.hpp"

class GearboxRenderer : public KineticBlockEntityRenderer {
public:
    mce::TexturePtr mGearboxTexture;
    mce::TexturePtr mHalfShaftTexture;

    GearboxRenderer() : KineticBlockEntityRenderer() {}

    virtual void loadTextures(std::shared_ptr<mce::TextureGroup> textures) override {
		KineticBlockEntityRenderer::loadTextures(textures);
        mGearboxTexture = textures->getTexture("textures/entity/gearbox", false, std::nullopt, cg::TextureSetLayerType::Normal);
        mHalfShaftTexture = textures->getTexture("textures/entity/axis", false, std::nullopt, cg::TextureSetLayerType::Normal);
    }

    virtual void renderSafe(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) const override {
        KineticBlockEntity& be = static_cast<KineticBlockEntity&>(data.entity);
        Facing::Axis boxAxis = getRotationAxisOf(be);
        float time = KineticBlockEntityRenderer::getTime();

        Vec3 renderPos = Vec3(be.getBlockPos()) - ctx.mCameraTargetPosition;
        MatrixStack& stack = ctx.mScreenContext.camera->worldMatrixStack;
        auto worldSpace = stack.push(); // push into local space
        worldSpace->translate(0.0f, -0.5f, 0.0f);

        
        auto shaftHalf = Models::partial(ctx.mScreenContext.tessellator, AllPartialModels::SHAFT_HALF);

        for (auto& dir : Facing::DIRECTIONS) {
            Facing::Axis axis = Facing::getAxis(dir);
            if (axis == boxAxis) continue;

            auto mat = stack.push();
            
            float offset = getRotationOffsetForPosition(be, be.mPosition, axis);
            float angle = glm::mod((time * be.getSpeed() * 3.0f / 10.0f), 360.0f);
            
            angle += offset;
            angle = angle / 180.0f * glm::pi<float>();
            
            rotateToFace(*mat, dir);
            kineticRotationTransform(be, *mat, axis, angle);
            mat->translate(renderPos.x + 0.5f, renderPos.y + 0.5f, renderPos.z + 0.5f);

            shaftHalf->render(ctx, self.getEntityMaterial());
            stack.pop();
        }

        auto gearboxModel = Models::partial(ctx.mScreenContext.tessellator, AllPartialModels::GEARBOX);
        worldSpace->translate(renderPos.x + 0.5f, renderPos.y + 0.5f, renderPos.z + 0.5f); // move to block position

        gearboxModel->render(ctx, self.getStaticEntityMaterial());

        stack.pop(); // pop back to world space
    }
};