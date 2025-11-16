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

void KineticBlockEntityRenderer::renderSafe(BlockActorRenderer &self, BaseActorRenderContext &ctx, BlockActorRenderData &data)
{
    // if (VisualizationManager.supportsVisualization(be.getLevel())) return;

    KineticBlockEntity& entity = static_cast<KineticBlockEntity&>(data.entity);
    const Block& block = getRenderedBlockState(entity);

    auto model = getModel(ctx.mScreenContext.tessellator, entity, block);
    renderRotatingBuffer(self, entity, ctx, model, mShaftTexture);

    // little util for blocks that have non-rotating parts, i.e. creative motor casing
    renderAdditional(self, ctx, data);
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
    FacingID face = Facing::fromDirectionAndAxis(Facing::AxisDirection::POSITIVE, axis);
    rotateVerticalToFace(mat, face);
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
    return Amethyst::GetClientCtx().mMinecraft->mSimTimer.mLastTimeSeconds * 20.0f; // to ticks to match java
}

float KineticBlockEntityRenderer::getTickAlpha()
{
    return Amethyst::GetClientCtx().mMinecraft->mSimTimer.mAlpha; // to ticks to match java
}

void KineticBlockEntityRenderer::rotateVerticalToFace(Matrix &mat, FacingID dir)
{
    switch (dir) {
        case Facing::Name::UP:
            // no rotation, identity
            break;
        case Facing::Name::DOWN:
            mat.rotateRad(glm::pi<float>(), 1, 0, 0); // flip upside-down around X
            break;
        case Facing::Name::NORTH:
            mat.rotateRad(glm::half_pi<float>(), 1, 0, 0); // rotate -90° X to lay flat on Z-
            break;
        case Facing::Name::SOUTH:
            mat.rotateRad(-glm::half_pi<float>(), 1, 0, 0); // rotate +90° X to lay flat on Z+
            break;
        case Facing::Name::EAST:
            mat.rotateRad(glm::half_pi<float>(), 0, 0, 1); // rotate +90° Z to lay flat on X+
            break;
        case Facing::Name::WEST:
            mat.rotateRad(-glm::half_pi<float>(), 0, 0, 1); // rotate -90° Z to lay flat on X-
            break;
        default:
            std::unreachable();
            break;
    }
}

void KineticBlockEntityRenderer::rotateToFace(Matrix &mat, FacingID dir)
{
    switch (dir) {
        case Facing::Name::NORTH:
            mat.rotateRad(glm::pi<float>(), 0, 1, 0); // flip 180° around Y
            break;
        case Facing::Name::SOUTH:
            // was 180°, now identity
            break;
        case Facing::Name::EAST:
            mat.rotateRad(glm::half_pi<float>(), 0, 1, 0); // flip +90° Y
            break;
        case Facing::Name::WEST:
            mat.rotateRad(-glm::half_pi<float>(), 0, 1, 0); // flip -90° Y
            break;
        case Facing::Name::UP:
            mat.rotateRad(-glm::half_pi<float>(), 1, 0, 0); // keep same
            break;
        case Facing::Name::DOWN:
            mat.rotateRad(glm::half_pi<float>(), 1, 0, 0); // keep same
            break;
        default:
            std::unreachable();
            break;
    }
}