#pragma once
#include "content/kinetics/base/KineticBlockEntity.hpp"

class GeneratingKineticBlockEntity : public KineticBlockEntity {
public:
    bool reActivateSource;

    GeneratingKineticBlockEntity(BlockActorType typeIn, const BlockPos& pos, const std::string& id)
		: KineticBlockEntity(typeIn, pos, id), reActivateSource(false) {}

protected:
    void notifyStressCapacityChange(float capacity) {
        getOrCreateNetwork()->updateCapacityFor(this, capacity);
    }

    virtual void removeSource() override {
        if (hasSource() && isSource()) {
            reActivateSource = true;
        } 
        KineticBlockEntity::removeSource();
    }

    virtual void setSource(const BlockPos& source) override {
        KineticBlockEntity::setSource(source);
        
        const BlockActor* blockEntity = level->mBlockSource->getBlockEntity(source);
		if (blockEntity == nullptr || !KineticBlockEntity::isKineticBlockEntity(*blockEntity)) return;

        const KineticBlockEntity& sourceBE = static_cast<const KineticBlockEntity&>(*blockEntity);
        if (reActivateSource && std::abs(sourceBE.getSpeed()) >= std::abs(getGeneratedSpeed())) {
            reActivateSource = false;
        }
    }

    virtual void tick(BlockSource& source) override {
        KineticBlockEntity::tick(source);

        if (reActivateSource) {
            updateGeneratedRotation();
            reActivateSource = false;
        }
    }

    void updateGeneratedRotation() {
        float speed = getGeneratedSpeed();
        float prevSpeed = this->speed;

        if (level == nullptr || level->isClientSide()) return;

		Log::Info("GeneratingKineticBlockEntity::updateGeneratedRotation called at {} with speed {}, previous speed {}", mPosition, speed, prevSpeed);

        if (prevSpeed != speed) {
            if (!hasSource()) {
                IRotate::SpeedLevel levelBefore = IRotate::SpeedLevel::of(this->speed);
                IRotate::SpeedLevel levelAfter = IRotate::SpeedLevel::of(speed);
                if (levelBefore != levelAfter) {
                    // effects.queueRotationIndicators(); seems to just be particles stuff
                }
            }

            applyNewSpeed(prevSpeed, speed);
        }

        if (hasNetwork() && speed != 0) {
            KineticNetwork* network = getOrCreateNetwork();
            notifyStressCapacityChange(calculateAddedStressCapacity());
            getOrCreateNetwork()->updateStressFor(this, calculateStressApplied());
            network->updateStress();
        }

        onSpeedChanged(prevSpeed);
        sendData();
    }

    void applyNewSpeed(float prevSpeed, float speed) {
        // Speed changed to 0
        if (speed == 0) {
            if (hasSource()) {
                notifyStressCapacityChange(0.0f);
                getOrCreateNetwork()->updateStressFor(this, calculateStressApplied());
                return;
            }

            detachKinetics();
            setSpeed(0.0f);
            setNetwork(std::nullopt);
            return;
        }

        // Now turning - create a new Network
        if (prevSpeed == 0) {
            setSpeed(speed);
            setNetwork(createNetworkId());
            attachKinetics();
            return;
        }

        // Change speed when overpowered by other generator
        if (hasSource()) {
            // Staying below Overpowered speed
            if (std::abs(prevSpeed) >= std::abs(speed)) {
                if (mce::Math::signum(prevSpeed) != mce::Math::signum(speed)) {
                    level->destroyBlock(mPosition, true);
                }
                return;
            }

            // Faster than the attached network -> become the new source
            detachKinetics();
            setSpeed(speed);
            source = std::nullopt;
            setNetwork(createNetworkId());
            attachKinetics();
            return;
        }

        // Reapply source
        detachKinetics();
        setSpeed(speed);
        attachKinetics();
    }

    uint64_t createNetworkId() {
        return mPosition.asLong();
    }
};