#pragma once
#include "content/kinetics/base/GeneratingKineticBlockEntity.hpp"
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include "content/kinetics/motor/KineticScrollValueBehaviour.hpp"

class CreativeMotorBlockEntity : public GeneratingKineticBlockEntity {
public:
    static const int DEFAULT_SPEED = 16;
    static const int MAX_SPEED = 256;

    std::shared_ptr<ScrollValueBehaviour> generatedSpeed;
	// AbstractComputerBehaviour computerBehaviour; // this is purely for computer craft integration

    CreativeMotorBlockEntity(const BlockPos& pos, const std::string& id)
        : GeneratingKineticBlockEntity(pos, id) {}

    virtual void addBehaviours(std::vector<std::shared_ptr<BlockEntityBehaviour>>& behavioursList) override {
        GeneratingKineticBlockEntity::addBehaviours(behavioursList);
        int max = MAX_SPEED;

        generatedSpeed = std::make_shared<KineticScrollValueBehaviour>(
            "kinetics.creative_motor.rotation_speed", this, std::make_shared<MotorValueBox>()
        );

		generatedSpeed->between(-max, max);
		generatedSpeed->value = DEFAULT_SPEED;
        generatedSpeed->withCallback([this](int newValue) {
            this->updateGeneratedRotation();
        });
        behavioursList.push_back(generatedSpeed);
    }

    virtual void initialize() override {
        GeneratingKineticBlockEntity::initialize();
        if (!hasSource() || getGeneratedSpeed() > getTheoreticalSpeed()) {
            updateGeneratedRotation();
        }
    }

    virtual float getGeneratedSpeed() const override;

    class MotorValueBox : public SidedValueBoxTransform {
    protected:
        virtual Vec3 getSouthLocation() const override {
            return Vec3::voxelSpace(8, 8, 12.5f);
        }

    public:
        virtual std::optional<Vec3> getLocalOffset(const BlockSource& region, const BlockPos& pos, const Block& state) const override {
            FacingID face = state.getState<FacingID>(VanillaStates::FacingDirection);

            return (
                SidedValueBoxTransform::getLocalOffset(region, pos, state).value() + Vec3::atLowerCornerOf(Facing::normal(face))
            ) * Vec3(-1.0f / 16.0f, -1.0f / 16.0f, -1.0f / 16.0f); // wtf even is this
        }

        virtual void rotate(const BlockSource& region, const BlockPos& pos, const Block& state, MatrixStack& ms) const override {
            SidedValueBoxTransform::rotate(region, pos, state, ms);
            FacingID face = state.getState<FacingID>(VanillaStates::FacingDirection);

            if (Facing::getAxis(face) == Facing::Axis::Y) return;
            if (getSide() != FacingID::UP) return;

            auto& mat = ms.getTop();
            mat.rotateZDegrees(Facing::getYAngle(face) + 180.0f);
        }

    protected:
        bool isSideActive(const Block& state, FacingID direction) const {
            FacingID facing = state.getState<FacingID>(VanillaStates::FacingDirection);
            if (Facing::getAxis(facing) != Facing::Axis::Y && direction == FacingID::DOWN) 
                return false;

            return Facing::getAxis(direction) != Facing::getAxis(facing);
        }
    };
};