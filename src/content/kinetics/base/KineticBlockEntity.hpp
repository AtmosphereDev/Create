#pragma once
#include "foundation/blockEntity/SmartBlockEntity.hpp"

class KineticBlockEntity : public SmartBlockEntity {
public:
    uint64_t network;
    BlockPos source;
    bool networkDirty;
    bool updateSpeed;
    int preventSpeedUpdate;

    //KineticEffectHandler effects;
	float speed;
	float capacity;
	float stress;
	bool overStressed;
	bool wasMoved;

    int flickerTally;
    int networkSize;
    int validationCountdown;
    float lastStressApplied;
    float lastCapacityProvided;

    // SequenceContext sequenceContext;

    // public KineticBlockEntity(BlockEntityType<?> typeIn, BlockPos pos, BlockState state) {
	// 	super(typeIn, pos, state);
	// 	effects = new KineticEffectHandler(this);
	// 	updateSpeed = true;
	// }

	virtual void initialize() override {
		if (hasNetwork() && !level.isClientSide) {
			KineticNetwork network = getOrCreateNetwork();
			if (!network.initialized)
				network.initFromTE(capacity, stress, networkSize);
			network.addSilently(this, lastCapacityProvided, lastStressApplied);
		}

		SmartBlockEntity::initialize();
	}

    virtual void tick() override {
        
    }
};