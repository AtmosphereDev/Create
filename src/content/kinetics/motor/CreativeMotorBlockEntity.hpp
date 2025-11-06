#pragma once
#include "content/kinetics/base/GeneratingKineticBlockEntity.hpp"
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include "foundation/blockEntity/behaviour/scrollValue/ScrollValueBehaviour.hpp"

class CreativeMotorBlockEntity : public GeneratingKineticBlockEntity {
public:
    static const int DEFAULT_SPEED = 16;
    static const int MAX_SPEED = 256;

    std::shared_ptr<ScrollValueBehaviour> generatedSpeed;
	// AbstractComputerBehaviour computerBehaviour; // this is purely for computer craft integration

    CreativeMotorBlockEntity(BlockActorType typeIn, const BlockPos& pos, const std::string& id)
        : GeneratingKineticBlockEntity(typeIn, pos, id) {
            Log::Info("Creative motor block entity created!!");
        }

    virtual void addBehaviours(std::vector<std::shared_ptr<BlockEntityBehaviour>>& behavioursList) override {
        GeneratingKineticBlockEntity::addBehaviours(behavioursList);
        int max = MAX_SPEED;

        generatedSpeed = std::make_shared<ScrollValueBehaviour>(
            "kinetics.creative_motor.rotation_speed", this, std::make_shared<MotorValueBox>()
        );

		// generatedSpeed = new KineticScrollValueBehaviour(CreateLang.translateDirect("kinetics.creative_motor.rotation_speed"),
		// 	this, new MotorValueBox());

		// generatedSpeed.between(-max, max);
		generatedSpeed->value = DEFAULT_SPEED;
        behavioursList.push_back(generatedSpeed);

		// generatedSpeed.withCallback(i -> this.updateGeneratedRotation());
		// behaviours.add(generatedSpeed);
		// behaviours.add(computerBehaviour = ComputerCraftProxy.behaviour(this));
    }

    virtual void initialize() override {
        GeneratingKineticBlockEntity::initialize();
        if (!hasSource() || getGeneratedSpeed() > getTheoreticalSpeed()) {
            updateGeneratedRotation();
            Log::Info("updateGeneratedRotation called in initialize");
        }
        Log::Info("Creative motor initialize!! generated speed {}, speed {}", getGeneratedSpeed(), speed);
    }

    virtual float getGeneratedSpeed() override {
        // if (!AllBlocks.CREATIVE_MOTOR.has(getBlockState())) why is this here?
		// 	return 0;

        return convertToDirection(generatedSpeed->getValue(), getBlock().getState<FacingID>(VanillaStates::FacingDirection));
    }

    class MotorValueBox : public ValueBoxTransform {
    public:
        
    };
};