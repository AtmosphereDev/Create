#include "KineticBlockEntityRenderer.hpp"
#include "AllBlocks.hpp"
#include "flywheel/api/model/Models.hpp"
#include "AllPartialModels.hpp"
#include "content/kinetics/base/RotatedPillarKineticBlock.hpp"
#include "content/kinetics/simpleRelays/ShaftBlock.hpp"
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include <mc/src-deps/renderer/Camera.hpp>
#include "content/kinetics/base/KineticBlockEntityVisual.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/epsilon.hpp>

void KineticBlockEntityRenderer::renderSafe(BlockActorRenderer &self, BaseActorRenderContext &ctx, BlockActorRenderData &data) const
{
    // if (VisualizationManager.supportsVisualization(be.getLevel())) return;

    KineticBlockEntity& entity = static_cast<KineticBlockEntity&>(data.entity);
    const Block& block = getRenderedBlockState(entity);

    auto model = getModel(ctx.mScreenContext.tessellator, entity, block);

    Vec3 renderPos = Vec3(entity.getBlockPos()) - ctx.mCameraTargetPosition;
    MatrixStack& stack = ctx.mScreenContext.camera->worldMatrixStack;
    auto mat = stack.push();

	mat->translate(renderPos.x + 0.5f, renderPos.y + 0.5f, renderPos.z + 0.5f); // on X and Z to center on block, on Y to make model centered (for rotating around center)

    renderAdditional(self, ctx, data);

    standardKineticRotationTransform(entity, *mat); // rotate around axis
	applyModelRotation(entity, *mat); // block rotation, i.e. facing negative Z axis would rotate it
    mat->translate(0, -0.5f, 0); // re-align the vertical

    for (const auto& mesh : model->meshes) {
        mesh.mesh.renderMesh(ctx.mScreenContext, self.getStaticEntityMaterial(), mShaftTexture);
    }


    stack.pop();
}

const Block &KineticBlockEntityRenderer::shaft(Facing::Axis axis)
{
    return *AllBlocks::SHAFT->getRenderBlock()
        .setState<Facing::Axis>(VanillaStates::PillarAxis, axis);
}

std::shared_ptr<Model> KineticBlockEntityRenderer::getModel(Tessellator &tess, const KineticBlockEntity &be, const Block &state) const
{
    return Models::partial(tess, AllPartialModels::SHAFT);
}

void KineticBlockEntityRenderer::applyModelRotation(const KineticBlockEntity &be, Matrix &mat) const
{
    Facing::Axis axis = getRotationAxisOf(be);
    switch (axis)
    {
        case Facing::Axis::X:
            mat.rotateRad(glm::half_pi<float>(), 0.0f, 0.0f, 1.0f);
            break;

        case Facing::Axis::Z:
            mat.rotateRad(glm::half_pi<float>(), 1.0f, 0.0f, 0.0f);
            break;

        case Facing::Axis::Y:
        default:
            break;
    }
}

void KineticBlockEntityRenderer::standardKineticRotationTransform(const KineticBlockEntity &be, Matrix& mat)
{
    Facing::Axis axis = getRotationAxisOf(be);
    kineticRotationTransform(be, mat, axis, getAngleForBe(be, be.getBlockPos(), axis));
}

void KineticBlockEntityRenderer::kineticRotationTransform(const KineticBlockEntity &be, Matrix &mat, Facing::Axis axis, float angle)
{
    auto direction = Facing::getOffset(Facing::fromDirectionAndAxis(Facing::AxisDirection::POSITIVE, axis));
    mat.rotateRad(angle, direction.x, direction.y, direction.z);
}

float KineticBlockEntityRenderer::getAngleForBe(const KineticBlockEntity &be, const BlockPos &pos, Facing::Axis axis)
{
    float time = getTime();
    float offset = getRotationOffsetForPosition(be, pos, axis);
    float angle = glm::mod(time * be.getSpeed() * 3.0f / 10.0f + offset, 360.0f) / 180.0f * glm::pi<float>();
    return angle;
}

float KineticBlockEntityRenderer::getRotationOffsetForPosition(const KineticBlockEntity &be, const BlockPos &pos, Facing::Axis axis)
{
    return KineticBlockEntityVisual<KineticBlockEntity>::rotationOffset(be.getBlock(), axis, pos) + be.getRotationAngleOffset(axis);
}

float KineticBlockEntityRenderer::getTime()
{
    return Amethyst::GetClientCtx().mMinecraft->mSimTimer.mLastTimeSeconds * 10000.0f;
}

void KineticBlockEntityRenderer::rotateToFace(Matrix &mat, FacingID dir)
{
    switch (dir) {
        case Facing::Name::NORTH:
            // default, no rotation needed
            break;
        case Facing::Name::SOUTH:
            mat.rotateRad(glm::pi<float>(), 0, 1, 0); // 180° around Y
            break;
        case Facing::Name::EAST:
            mat.rotateRad(glm::half_pi<float>(), 0, 1, 0); // +90° around Y
            break;
        case Facing::Name::WEST:
            mat.rotateRad(-glm::half_pi<float>(), 0, 1, 0); // -90° around Y
            break;
        case Facing::Name::UP:
            mat.rotateRad(-glm::half_pi<float>(), 1, 0, 0); // rotate -90° around X
            break;
        case Facing::Name::DOWN:
            mat.rotateRad(glm::half_pi<float>(), 1, 0, 0); // rotate +90° around X
            break;
        default:
            std::unreachable();
            break;
    }
}
