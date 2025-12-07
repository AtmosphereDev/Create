#pragma once
#include "content/kinetics/base/KineticBlockEntity.hpp"

class GeneratingKineticBlockEntity : public KineticBlockEntity {
public:
    bool reActivateSource;

    GeneratingKineticBlockEntity(const BlockPos& pos, const std::string& id)
		: KineticBlockEntity(pos, id), reActivateSource(false) {}

protected:
    void notifyStressCapacityChange(float capacity) {
        getOrCreateNetwork()->updateCapacityFor(this->getShared<KineticBlockEntity>(), capacity);
    }

    virtual void removeSource() override {
        Log::Info("GeneratingKineticBlockEntity at {} removing source (type: {})", mPosition, getBlock().getFullName().getString());
        if (hasSource() && isSource()) {
            reActivateSource = true;
            Log::Info("reActivateSource set to true for GeneratingKineticBlockEntity at {} (type: {})", mPosition, getBlock().getFullName().getString());
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
            Log::Info("reActivateSource set to false for GeneratingKineticBlockEntity at {} (type: {})", mPosition, getBlock().getFullName().getString());
        }
    }

    virtual void tick(BlockSource& region) override {
        KineticBlockEntity::tick(region);

        if (level == nullptr) {
			Log::Warning("GeneratingKineticBlockEntity tick called with null level at {}", mPosition);
			return;
        }

        if (reActivateSource) {
            updateGeneratedRotation();
            reActivateSource = false;
			Log::Info("reActivateSource set to false for GeneratingKineticBlockEntity at {} (type: {})", mPosition, getBlock().getFullName().getString());
        }
    }

    void updateGeneratedRotation() {
        float speed = getGeneratedSpeed();
        float prevSpeed = this->speed;
        Log::Info("GeneratingKineticBlockEntity at {} updating generated speed from {} to {}", mPosition, prevSpeed, speed);

        if (level == nullptr || level->isClientSide()) return;

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
            getOrCreateNetwork()->updateStressFor(this->getShared<KineticBlockEntity>(), calculateStressApplied());
            network->updateStress();
        }

        onSpeedChanged(prevSpeed);
        sendData();

        Log::Info("GeneratingKineticBlockEntity at {} applied new speed {}", mPosition, this->speed);
    }

    void applyNewSpeed(float prevSpeed, float speed) {
        Log::Info("GeneratingKineticBlockEntity at {} changing speed from {} to {}", mPosition, prevSpeed, speed);
        
        // Speed changed to 0
        if (speed == 0) {
            if (hasSource()) {
                notifyStressCapacityChange(0.0f);
                getOrCreateNetwork()->updateStressFor(this->getShared<KineticBlockEntity>(), calculateStressApplied());
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
            // removeSource(); // original code just sets source to null, but this doesnt update GeneratingKineticBlockEntity

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