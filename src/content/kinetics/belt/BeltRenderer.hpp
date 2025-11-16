#pragma once
#include "content/kinetics/base/KineticBlockEntityRenderer.hpp"
#include "flywheel/api/model/Models.hpp"
#include "AllPartialModels.hpp"
#include "content/kinetics/belt/BeltBlock.hpp"
#include "content/kinetics/belt/BeltBlockEntity.hpp"
#include "AllBlocks.hpp"
#include <mc/src-deps/minecraftrenderer/renderer/GlobalConstantBuffers.hpp>
#include <mc/src-client/common/client/renderer/RenderMaterialGroup.hpp>

class BeltRenderer : public KineticBlockEntityRenderer {
public:
    mce::MaterialPtr mBeltMaterial;

    mce::MaterialPtr& getBeltMaterial() {
        if (mBeltMaterial.isNull()) {
            mBeltMaterial = mce::RenderMaterialGroup::switchable.getMaterial("mechanical_belt");
            Log::Info("Loaded belt material {}", mBeltMaterial.mRenderMaterialInfoPtr->mHashedName);
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
        bool diagonal = BeltSlope::isDiagonal(beltSlope);
        bool start = part == BeltPart::START;
        bool end = part == BeltPart::END;
        bool sideways = beltSlope == BeltSlope::SIDEWAYS;
        bool alongX = Facing::getAxis(facing) == Facing::Axis::X;

        MatrixStack& stack = ctx.mScreenContext.camera->worldMatrixStack;
        auto msr = stack.push();
        msr->translate(0.0f, -0.5f, 0.0f); // .center
        msr->rotateYDegrees(Facing::horizontalAngle(facing) + (upward ? 180.0f : 0.0f) + (sideways ? 270.0f : 0.0f));
        msr->rotateZDegrees(sideways ? 90.0f : 0.0f);
        msr->rotateXDegrees(!diagonal && beltSlope != BeltSlope::HORIZONTAL ? 90.0f : 0.0f);
        msr->translate(0.0, 0.5f, 0.0f); // .uncenter

        if (downward || beltSlope == BeltSlope::VERTICAL && axisDirection == Facing::AxisDirection::POSITIVE) {
            bool b = start;
            start = end;
            end = b;
        }

        auto beltMat = stack.push();
        beltMat->translate(renderPos.x + 0.5f, renderPos.y, renderPos.z + 0.5f);

        for (bool bottom : {false, true}) {
            auto beltPartial = getBeltPartial(diagonal, start, end, bottom);
            auto beltBuffer = Models::partial(ctx.mScreenContext.tessellator, beltPartial);

            auto* uvOffset = ctx.mScreenContext.constantBuffers.entityConstantBuffer.UV_ANIM;
            auto data = uvOffset->getData();

            // UV shift
            float speed = be.getSpeed(); // temp hardcoded
            if (speed != 0) {
                float time = getTime() * Facing::getStep(axisDirection);
                if (diagonal && (downward ^ alongX) || !sideways && !diagonal && alongX 
                    || sideways && axisDirection == Facing::AxisDirection::NEGATIVE) {
                    speed = -speed;
                }

                float scrollMult = diagonal ? 3.0f / 8.0f : 0.5f;
                float spriteSize = 16.0f; // hardcoded 16x16 texture size, bleh
                float scroll = speed * time / (31.5f * 16.0f) + (bottom ? 0.5f : 0.0f);
                scroll = scroll - std::floor(scroll);
                scroll = scroll * spriteSize * scrollMult;

                data.y = scroll;
                uvOffset->setData(data);
            }
            
            beltBuffer->render(ctx, getBeltMaterial());

            // Diagonal belt do not have a separate bottom model
            if (diagonal)
                break;
        }

        stack.pop();

        // if has pulley
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