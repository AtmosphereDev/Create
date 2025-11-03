#pragma once
#include "foundation/blockEntity/renderer/SafeBlockEntityRenderer.hpp"
#include <mc/src/common/world/Facing.hpp>
#include "AllBlocks.hpp"
#include <mc/src-deps/minecraftrenderer/renderer/Mesh.hpp>
#include <mc/src-client/common/client/renderer/screen/ScreenContext.hpp>
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src/common/util/Timer.hpp>
#include <mc/src/common/Minecraft.hpp>

const Block;
class Model;

class KineticBlockEntityRenderer : public SafeBlockEntityRenderer {
public:
    virtual void renderSafe(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) const override;

protected:
    virtual const Block& getRenderedBlockState(const KineticBlockEntity& entity) const {
        return entity.getBlock();
    }

    std::shared_ptr<Model> getRotatedModel(Tessellator& tess, const KineticBlockEntity& be, const Block& state);

public:
    static const Block& shaft(Facing::Axis axis);

    static Facing::Axis getRotationAxisOf(const KineticBlockEntity& entity) {
        const Block& block = entity.getBlock();
        IRotate& rotate = (IRotate&)*block.mLegacyBlock;
        return rotate.getRotationAxis(block);
    }

    static void standardKineticRotationTransform(const KineticBlockEntity& be, Matrix& mat);

    static void kineticRotationTransform(const KineticBlockEntity& be, Matrix& mat, Facing::Axis axis, float angle);

    static float getAngleForBe(const KineticBlockEntity& be, const BlockPos& pos, Facing::Axis axis);

    static float getRotationOffsetForPosition(const KineticBlockEntity& be, const BlockPos& pos, Facing::Axis axis);
};