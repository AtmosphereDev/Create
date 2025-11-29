#pragma once
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"
#include "flywheel/api/model/Models.hpp"
#include "AllPartialModels.hpp"
#include "content/kinetics/belt/BeltBlock.hpp"
#include "content/kinetics/belt/BeltBlockEntity.hpp"
#include "AllBlocks.hpp"
#include <mc/src-deps/minecraftrenderer/renderer/GlobalConstantBuffers.hpp>
#include <mc/src-client/common/client/renderer/RenderMaterialGroup.hpp>
#include <mc/src-client/common/client/renderer/game/ItemInHandRenderer.hpp>
#include <mc/src-client/common/client/world/item/ItemIconManager.hpp>
#include <mc/src-client/common/client/renderer/ActorShaderManager.hpp>
#include <mc/src/common/world/phys/Vec4.hpp>

class BeltRenderer : public KineticBlockEntityRenderer {
public:
    mce::MaterialPtr mBeltMaterial;

    mce::MaterialPtr& getBeltMaterial() {
        if (mBeltMaterial.isNull()) {
            mBeltMaterial = mce::RenderMaterialGroup::switchable.getMaterial("mechanical_belt");
        }

        return mBeltMaterial;
    }

    virtual void renderSafe(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) override {
        BeltBlockEntity& be = static_cast<BeltBlockEntity&>(data.entity);
        Vec3 renderPos = Vec3(be.getBlockPos()) - ctx.mCameraTargetPosition;

        const Block& blockState = be.getBlock();
        if (AllBlocks::BELT != blockState.mLegacyBlock) 
            return;

        BeltSlope::Type beltSlope = blockState.getState<BeltSlope::Type>(BeltBlock::SLOPE());
        BeltPart::Type part = blockState.getState<BeltPart::Type>(BeltBlock::PART());
        FacingID facing = blockState.getState<FacingID>(HorizontalKineticBlock::HORIZONTAL_FACING());
        Facing::AxisDirection axisDirection = Facing::getAxisDirection(facing);

        bool downward = beltSlope == BeltSlope::DOWNWARD;
        bool upward = beltSlope == BeltSlope::UPWARD;
        bool diagonal = downward || upward;
        bool start = part == BeltPart::START;
        bool end = part == BeltPart::END;
        bool sideways = beltSlope == BeltSlope::SIDEWAYS;
        bool alongX = Facing::getAxis(facing) == Facing::Axis::X;

        MatrixStack& stack = ctx.mScreenContext.camera->worldMatrixStack;
        auto msr = stack.push();
        msr->translate(0.0f, 0.5f, 0.0f); // .center
        msr->rotateYDegrees(Facing::horizontalAngle(facing) + (upward ? 180.0f : 0.0f) + (sideways ? 270.0f : 0.0f)); 
        msr->rotateZDegrees(sideways ? 90.0f : 0.0f);
        msr->rotateXDegrees(!diagonal && beltSlope != BeltSlope::HORIZONTAL ? 90.0f : 0.0f);
        msr->translate(0.0, -0.5f, 0.0f); // .uncenter

        if (downward || beltSlope == BeltSlope::VERTICAL && axisDirection == Facing::AxisDirection::POSITIVE) {
            bool b = start;
            start = end;
            end = b;
        }
        
        float renderTime = getTime() / 8.0f;
        // Vec4 test = Vec4::Vec4();

        // ActorShaderManager::setupShaderParameters(
        //     ctx.mScreenContext,
        //     data.renderSource,
        //     be.mPosition,
        //     0.0f,
        //     true,
        //     *ctx.mClientInstance.mLightTexture.get(),
        //     Vec2::ONE,
        //     test
        // );

        for (bool bottom : {false, true}) {
            auto beltPartial = getBeltPartial(diagonal, start, end, bottom);
            auto beltBuffer = Models::partial(ctx.mScreenContext.tessellator, beltPartial);

            auto* uvOffset = ctx.mScreenContext.constantBuffers.entityConstantBuffer.UV_ANIM;
            glm::vec4 data = uvOffset->getData();

            // UV shift
            float speed = be.getSpeed();
            if (speed != 0) {
                float time = renderTime * Facing::getStep(axisDirection);
                if (diagonal && (downward ^ alongX) || !sideways && !diagonal && alongX 
                    || sideways && axisDirection == Facing::AxisDirection::NEGATIVE) {
                    speed = -speed;
                }

                float scrollMult = diagonal ? 3.0f / 8.0f : 0.5f;
                float spriteSize = 1.0f; 
                float scroll = speed * time / (31.5f * 16.0f) + (bottom ? 0.5f : 0.0f);
                scroll = scroll - std::floor(scroll);
                scroll = scroll * spriteSize * scrollMult;

                data.y = scroll * 16.0f;
                uvOffset->setData(data);
            }
            
            auto beltMat = stack.push();
            beltMat->translate(renderPos.x + 0.5f, renderPos.y, renderPos.z + 0.5f);
            beltBuffer->render(ctx, getBeltMaterial());
            stack.pop();

            // Diagonal belt do not have a separate bottom model
            if (diagonal)
                break;
        }

        if (be.hasPulley()) {
            FacingID dir = sideways ? FacingID::UP
                : Facing::getClockWise(blockState.getState<FacingID>(HorizontalKineticBlock::HORIZONTAL_FACING()));

            auto pulley = Models::partial(ctx.mScreenContext.tessellator, AllPartialModels::BELT_PULLEY);
            auto pulleyStack = stack.push();

            pulleyStack->translate(0.0f, -0.5f, 0.0f); // .center
            if (Facing::getAxis(dir) == Facing::Axis::Y) pulleyStack->rotateYDegrees(90.0f);
            else if (Facing::getAxis(dir) == Facing::Axis::X) pulleyStack->rotateZDegrees(90.0f);
            pulleyStack->rotateXDegrees(90.0f);
        
            standardKineticRotationTransform(be, *pulleyStack);

            pulleyStack->translate(renderPos.x + 0.5f, renderPos.y + 0.5f, renderPos.z + 0.5f);
            pulley->render(ctx, self.getEntityMaterial());

            stack.pop();
        }

        stack.pop();

        renderItems(be, getTickAlpha(), stack, ctx);
    }

    void renderItems(BeltBlockEntity& be, float partialTicks, MatrixStack& ms, BaseActorRenderContext& ctx) {
        if (!be.isController())
            return;
        if (be.beltLength == 0)
            return;

        auto mat = ms.push();

        FacingID beltFacing = be.getBeltFacing();
        BlockPos directionVec = Facing::normal(beltFacing);
        Vec3 beltStartOffset = Vec3::atLowerCornerOf(directionVec) * -0.5f + Vec3(0.5f, 15.0f / 16.0f, 0.5f); 
        mat->translate(beltStartOffset);

        BeltSlope::Type slope = be.getBlock().getState<BeltSlope::Type>(BeltBlock::SLOPE());
        int verticality = slope == BeltSlope::DOWNWARD ? -1 : (slope == BeltSlope::UPWARD ? 1 : 0);
        bool slopeAlongX = Facing::getAxis(beltFacing) == Facing::Axis::X;
        bool onContraption = false; // todo

        const auto& inventory = be.getInventory();
        for (const auto& transported : inventory->getTransportedItems()) {
            renderItem(be, partialTicks, ms, ctx, beltFacing, directionVec, slope, verticality, slopeAlongX, onContraption, *transported.get(), beltStartOffset);
        }

        const auto& lazyItem = inventory->getLazyClientItem();
        if (lazyItem.has_value()) {
            renderItem(be, partialTicks, ms, ctx, beltFacing, directionVec, slope, verticality, slopeAlongX, onContraption, lazyItem.value(), beltStartOffset);
        }

        ms.pop();
    }

    void renderItem(BeltBlockEntity& be, float partialTicks, MatrixStack& ms, BaseActorRenderContext& ctx, FacingID beltFacing, BlockPos directionVec, BeltSlope::Type slope, 
        int verticality, bool slopeAlongX, bool onContraption, const TransportedItemStack& transported, Vec3 beltStartOffset) 
    {
        float offset = std::lerp(transported.prevBeltPosition, transported.beltPosition, partialTicks);
        float sideOffset = std::lerp(transported.prevSideOffset, transported.sideOffset, partialTicks);
        float verticalMovement = verticality;

        if (be.getSpeed() == 0) {
            offset = transported.beltPosition;
            sideOffset = transported.sideOffset;
        }

        if (offset < 0.5f) {
            verticalMovement = 0;
        }
        else {
            verticalMovement = verticality * (std::min(offset, be.beltLength - 0.5f) - 0.5f);
        }

        Vec3 offsetVec = Vec3::atLowerCornerOf(directionVec) * offset;
        if (verticalMovement != 0) {
            offsetVec.y += verticalMovement;
        }
        bool onSlope = slope != BeltSlope::HORIZONTAL && std::clamp(offset, 0.5f, be.beltLength - 0.5f) == offset;
        bool tiltForward = (slope == BeltSlope::DOWNWARD 
            ^ Facing::getAxisDirection(beltFacing) == Facing::AxisDirection::POSITIVE) == (Facing::getAxis(beltFacing) == Facing::Axis::Z);
        float slopeAngle = onSlope ? (tiltForward ? -45.5f : 45.5f) : 0.0f;

        Vec3 itemPos = beltStartOffset + be.getBlockPos() + offsetVec;

        auto itemMat = ms.push();

        //ResolvedItemIconInfo iconInfo = transported.stack.getItem()->getIconInfo(transported.stack, 0, false);
		TextureAtlasItem* atlasItem = ItemIconManager::getIcon(transported.stack.getItem()->getIconInfo(transported.stack, 0, false));
        float iconWidth = atlasItem->pixelWidth();

        itemMat->scale(0.4f / iconWidth);

        // todo nudge angle
        itemMat->translate(itemPos);

        bool alongX = Facing::getAxis(Facing::getClockWise(beltFacing)) == Facing::Axis::X;
        if (!alongX)
            sideOffset *= -1.0f;

        itemMat->translate(alongX ? sideOffset : 0.0f, 0.0f, alongX ? 0.0f : sideOffset);
        itemMat->translate(ctx.mCameraTargetPosition * -1.0f); // camera adjust

        const ItemRenderCall& renderCall = ctx.mItemInHandRenderer.getRenderCallAtFrame(ctx, transported.stack, 0);
        if (ctx.mScreenContext.tessellator.mForceTessellateIntercept) {
            Log::Warning("Force intercepted item rendering is not implemented! Item name: '{}'", transported.stack.getItem()->mFullName);
        }
        else {
            ctx.mItemInHandRenderer.renderObject(ctx, renderCall, dragon::RenderMetadata(be.getBlockPos()), ItemContextFlags::None);
        }

        ms.pop();
    }

    static std::shared_ptr<PartialModel> getBeltPartial(bool diagonal, bool start, bool end, bool bottom) {
        if (diagonal) {
			if (start)
				return AllPartialModels::BELT_DIAGONAL_START;
			if (end)
				return AllPartialModels::BELT_DIAGONAL_END;
			return AllPartialModels::BELT_DIAGONAL_MIDDLE;
		} else if (bottom) {
			if (start)
				return AllPartialModels::BELT_START_BOTTOM;
			if (end)
				return AllPartialModels::BELT_END_BOTTOM;
			return AllPartialModels::BELT_MIDDLE_BOTTOM;
		} else {
			if (start)
				return AllPartialModels::BELT_START;
			if (end)
				return AllPartialModels::BELT_END;
			return AllPartialModels::BELT_MIDDLE;
		}
    }
};