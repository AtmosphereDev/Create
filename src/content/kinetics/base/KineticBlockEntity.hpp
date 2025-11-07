#pragma once
#include <mc/src/common/world/level/dimension/Dimension.hpp>
#include "foundation/blockEntity/SmartBlockEntity.hpp"
#include "api/stress/BlockStressValues.hpp"
#include "content/kinetics/KineticNetwork.hpp"
#include "content/kinetics/base/IRotate.hpp"
#include "content/kinetics/RotationPropagator.hpp"
#include <mc/src-deps/core/math/Math.hpp>
#include <infrastructure/config/AllConfigs.hpp>
#include <mc/src/common/nbt/NbtUtils.hpp>

class KineticBlockEntity : public SmartBlockEntity {
public:
    std::optional<uint64_t> network;
    std::optional<BlockPos> source;
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

    KineticBlockEntity(BlockActorType typeIn, const BlockPos& pos, const std::string& id)
		: SmartBlockEntity(typeIn, pos, id), speed(0), capacity(0), stress(0), overStressed(false), wasMoved(false), flickerTally(0),
		  networkSize(0), validationCountdown(0), lastStressApplied(0), lastCapacityProvided(0)
	{
		// effects = new KineticEffectHandler(this);
		updateSpeed = true;
	}

	virtual void initialize() override {
		if (hasNetwork() && !level->mLevel->isClientSide()) {
			KineticNetwork* network = getOrCreateNetwork();
			if (!network->initialized)
				network->initFromTE(capacity, stress, networkSize);

			network->addSilently(this, lastCapacityProvided, lastStressApplied);
		}

		SmartBlockEntity::initialize();
	}

    virtual void tick(BlockSource& source) override {
		//Assert(level != nullptr, "Level should not be null in KineticBlockEntity tick");
		if (!level) { 
			Log::Error("KineticBlockEntity tick called with null level at {}", mPosition);
			return;
		}

        if (!level->mLevel->isClientSide() && needsSpeedUpdate())
			attachKinetics();

		SmartBlockEntity::tick(source);
		// effects.tick();

		preventSpeedUpdate = 0;

		if (level->mLevel->isClientSide()) {
			// CatnipServices.PLATFORM.executeOnClientOnly(() -> () -> this.tickAudio()); TODO
			return;
		}

		if (validationCountdown-- <= 0) {
			 validationCountdown = AllConfigs::server().kinetics.kineticValidationFrequency.get();
			
			validateKinetics();
		}

		if (getFlickerScore() > 0)
			flickerTally = getFlickerScore() - 1;

		if (networkDirty) {
			if (hasNetwork()) getOrCreateNetwork()->updateNetwork();
			networkDirty = false;
		}
    }

	void validateKinetics() {
		if (hasSource()) {
			if (!hasNetwork()) {
				removeSource();
				return;
			}

			if (!level->mBlockSource->areChunksFullyLoaded(source.value(), 1)) 
				return;

			const BlockActor* blockEntity = level->mBlockSource->getBlockEntity(source.value());
			if (!blockEntity || !KineticBlockEntity::isKineticBlockEntity(*blockEntity) || static_cast<const KineticBlockEntity&>(*blockEntity).speed == 0) {
				removeSource();
				detachKinetics();
				return;
			}

			return;
		}

		if (speed != 0) {
			if (getGeneratedSpeed() == 0) {
				speed = 0;
			}
		}
	}

	void updateFromNetwork(float maxStress, float currentStress, int networkSize) {
		networkDirty = false;
		capacity = maxStress;
		stress = currentStress;
		networkSize = networkSize;
		bool overStressed = maxStress < currentStress && IRotate::StressImpact::isEnabled();
		setChanged();

		if (overStressed != this->overStressed) {
			float prevSpeed = getSpeed();
			this->overStressed = overStressed;
			onSpeedChanged(prevSpeed);
			sendData();
		}
	}

	// KineticsChangeEvent makeComputerKineticsChangeEvent() {
	// 	return new KineticsChangeEvent(speed, capacity, stress, overStressed);
	// }

	const Block& getStressConfigKey() {
		return getBlock();
	}

	float calculateStressApplied() {
		float impact = (float)BlockStressValues::getImpact(getStressConfigKey());
		this->lastStressApplied = impact;
		return impact;
	}

	float calculateAddedStressCapacity() {
		float capacity = (float)BlockStressValues::getCapacity(getStressConfigKey());
		this->lastCapacityProvided = capacity;
		return capacity;
	}

	void onSpeedChanged(float previousSpeed) {
		bool fromOrToZero = (previousSpeed == 0) != (getSpeed() == 0);
		Log::Info("KineticBlockEntity onSpeedChanged called, fromOrToZero: {}, previousSpeed: {}, newSpeed: {}", fromOrToZero, previousSpeed, getSpeed());

		bool directionSwap = !fromOrToZero && mce::Math::signum(previousSpeed) != mce::Math::signum(getSpeed());
		if (fromOrToZero || directionSwap)
			flickerTally = getFlickerScore() + 5;

		setChanged();
	}

	virtual void remove() override {
		// patchwork fix temporarily for level not existing on loaded block actors
		if (!level) return;

		if (!level->mLevel->isClientSide()) {
			if (hasNetwork())
				getOrCreateNetwork()->remove(this);
			detachKinetics();
		}
		SmartBlockEntity::remove();
	}

	// equivallent of write in original code
	virtual std::unique_ptr<BlockActorDataPacket> _getUpdatePacket(BlockSource& unk0) override {
		CompoundTag compound;

		compound.putFloat("Speed", speed);

		// if (sequenceContext != null && (!clientPacket || syncSequenceContext()))
		// 	compound.put("Sequence", sequenceContext.serializeNBT());

		if (needsSpeedUpdate())
			compound.putByte("NeedsSpeedUpdate", 1);

		if (hasSource())
			compound.put("Source", NbtUtils::writeBlockPos(source.value()));

		if (hasNetwork()) {
			CompoundTag networkTag;
			networkTag.putInt64("Id", static_cast<int64_t>(network.value()));
			networkTag.putFloat("Stress", stress);
			networkTag.putFloat("Capacity", capacity);
			networkTag.putInt("Size", networkSize);

			if (lastStressApplied != 0)
				networkTag.putFloat("AddedStress", lastStressApplied);

			if (lastCapacityProvided != 0)
				networkTag.putFloat("AddedCapacity", lastCapacityProvided);

			compound.put("Network", std::move(networkTag));
		}

		return std::make_unique<BlockActorDataPacket>(mPosition, std::move(compound));
	}

	// equivallent of read in original code
	virtual void _onUpdatePacket(const CompoundTag& compound, BlockSource& region) override {
		bool overStressedBefore = overStressed;
		clearKineticInformation();

		if (wasMoved) {
			return;
		}

		speed = compound.getFloat("Speed");
		// sequenceContext = SequenceContext.fromNBT(compound.getCompound("Sequence"));

		source = std::nullopt;
		if (compound.contains("Source")) {
			source = NbtUtils::readBlockPos(*compound.getCompound("Source"));
		}

		if (compound.contains("Network")) {
			const CompoundTag& networkTag = *compound.getCompound("Network");
			network = static_cast<uint64_t>(networkTag.getInt64("Id"));
			stress = networkTag.getFloat("Stress");
			capacity = networkTag.getFloat("Capacity");
			networkSize = networkTag.getInt("Size");

			lastStressApplied = 0;
			lastCapacityProvided = 0;

			if (networkTag.contains("LastStressApplied"))
				lastStressApplied = networkTag.getFloat("LastStressApplied");

			if (networkTag.contains("LastCapacityProvided"))
				lastCapacityProvided = networkTag.getFloat("LastCapacityProvided");

			overStressed = capacity < stress && IRotate::StressImpact::isEnabled();
		}

		Log::Info("KineticBlockEntity onUpdatePacket called, speed: {}, hasNetwork: {}, overStressed: {}", speed, hasNetwork(), overStressed);
	}

	bool needsSpeedUpdate() {
		return updateSpeed;
	}

	virtual float getGeneratedSpeed() {
		return 0;
	}

	bool isSource() {
		return getGeneratedSpeed() != 0;
	}

	float getSpeed() const {
		// if (overStressed || (level != nullptr && level->tickRateManager().isFrozen()))
		// 	return 0;
		// TODO: add back in tick rate manager freeze check

		if (overStressed)
			return 0;

		return getTheoreticalSpeed();
	}

	float getTheoreticalSpeed() const {
		return speed;
	}

	void setSpeed(float speed) {
		Log::Info("KineticBlockEntity setSpeed called with speed {}", speed);
		this->speed = speed;
	}

	bool hasSource() {
		return source != std::nullopt;
	}

	virtual void setSource(const BlockPos& source) {
		this->source = source;
		if (level == nullptr || level->isClientSide())
			return;

		const BlockActor* blockEntity = level->mBlockSource->getBlockEntity(source);
		if (blockEntity == nullptr || !KineticBlockEntity::isKineticBlockEntity(*blockEntity)) {
			removeSource();
			return;
		}

		const KineticBlockEntity& sourceBE = static_cast<const KineticBlockEntity&>(*blockEntity);
		setNetwork(sourceBE.network);
		copySequenceContextFrom(sourceBE);
	}

	void copySequenceContextFrom(const KineticBlockEntity& sourceBE) {
		// sequenceContext = sourceBE.sequenceContext;
	}

	virtual void removeSource() {
		float prevSpeed = getSpeed();

		speed = 0;
		source = std::nullopt;
		setNetwork(std::nullopt);
		// sequenceContext = null;

		onSpeedChanged(prevSpeed);
	}

	void setNetwork(std::optional<uint64_t> networkIn) {
		if (network == networkIn)
			return;

		if (network != std::nullopt)
			getOrCreateNetwork()->remove(this);

		network = networkIn;
		setChanged();

		if (networkIn == std::nullopt)
			return;

		network = networkIn;
		KineticNetwork* network = getOrCreateNetwork();
		network->initialized = true;
		network->add(this);
	}

	KineticNetwork* getOrCreateNetwork();

	bool hasNetwork() {
		return network != std::nullopt;
	}

	void attachKinetics() {
		updateSpeed = false;
		RotationPropagator::handleAdded(*level, mPosition, *this);
	}

	void detachKinetics() {
	// 	RotationPropagator.handleRemoved(level, worldPosition, this);
		// RotationPropagator::handleRemoved(level, mPosition, this);
	}

	// boolean isSpeedRequirementFulfilled() {
	// 	BlockState state = getBlockState();
	// 	if (!(getBlockState().getBlock() instanceof IRotate))
	// 		return true;
	// 	IRotate def = (IRotate) state.getBlock();
	// 	SpeedLevel minimumRequiredSpeedLevel = def.getMinimumRequiredSpeedLevel();
	// 	return Math.abs(getSpeed()) >= minimumRequiredSpeedLevel.getSpeedValue();
	// }

	// 	public static void switchToBlockState(Level world, BlockPos pos, BlockState state) {
	// 	if (world.isClientSide)
	// 		return;

	// 	BlockEntity blockEntity = world.getBlockEntity(pos);
	// 	BlockState currentState = world.getBlockState(pos);
	// 	boolean isKinetic = blockEntity instanceof KineticBlockEntity;

	// 	if (currentState == state)
	// 		return;
	// 	if (blockEntity == null || !isKinetic) {
	// 		world.setBlock(pos, state, Block.UPDATE_ALL);
	// 		return;
	// 	}

	// 	KineticBlockEntity kineticBlockEntity = (KineticBlockEntity) blockEntity;
	// 	if (state.getBlock() instanceof KineticBlock
	// 		&& !((KineticBlock) state.getBlock()).areStatesKineticallyEquivalent(currentState, state)) {
	// 		if (kineticBlockEntity.hasNetwork())
	// 			kineticBlockEntity.getOrCreateNetwork()
	// 				.remove(kineticBlockEntity);
	// 		kineticBlockEntity.detachKinetics();
	// 		kineticBlockEntity.removeSource();
	// 	}

	// 	if (blockEntity instanceof GeneratingKineticBlockEntity generatingBlockEntity) {
	// 		generatingBlockEntity.reActivateSource = true;
	// 	}

	// 	world.setBlock(pos, state, Block.UPDATE_ALL);
	// }

	virtual void addBehaviours(std::vector<std::shared_ptr<BlockEntityBehaviour>>& behavioursList) override {}

	void clearKineticInformation() {
		speed = 0;
		source = std::nullopt;
		network = 0;
		overStressed = false;
		stress = 0;
		capacity = 0;
		lastStressApplied = 0;
		lastCapacityProvided = 0;
	}

	void warnOfMovement() {
		wasMoved = true;
	}

	int getFlickerScore() {
		return flickerTally;
	}

	static float convertToDirection(float axisSpeed, FacingID d) {
		return Facing::getAxisDirection(d) == Facing::AxisDirection::POSITIVE ? axisSpeed : -axisSpeed;
	}

	static float convertToLinear(float speed) {
		return speed / 512.0f;
	}

	static float convertToAngular(float speed) {
		return speed * 3 / 10.0f;
	}

	bool isOverStressed() {
		return overStressed;
	}

	// Custom propagation

		/**
	 * Specify ratio of transferred rotation from this kinetic component to a
	 * specific other.
	 *
	 * @param target           other Kinetic BE to transfer to
	 * @param stateFrom        this BE's blockstate
	 * @param stateTo          other BE's blockstate
	 * @param diff             difference in position (to.pos - from.pos)
	 * @param connectedViaAxes whether these kinetic blocks are connected via mutual
	 *                         IRotate.hasShaftTowards()
	 * @param connectedViaCogs whether these kinetic blocks are connected via mutual
	 *                         IRotate.hasIntegratedCogwheel()
	 * @return factor of rotation speed from this BE to other. 0 if no rotation is
	 * transferred, or the standard rules apply (integrated shafts/cogs)
	 */
	// float propagateRotationTo(KineticBlockEntity target, BlockState stateFrom, BlockState stateTo, BlockPos diff,
	// 								 boolean connectedViaAxes, boolean connectedViaCogs) {
	// 	return 0;
	// }

	/**
	 * Specify additional locations the rotation propagator should look for
	 * potentially connected components. Neighbour list contains offset positions in
	 * all 6 directions by default.
	 *
	 * @param block
	 * @param state
	 * @param neighbours
	 * @return
	 */
	std::vector<BlockPos> addPropagationLocations(IRotate& block, const Block& state, std::vector<BlockPos> neighbours) {
		if (!canPropagateDiagonally(block, state))
			return neighbours;

		Facing::Axis axis = block.getRotationAxis(state);

		for (auto& offset : BlockPos::betweenClosed(BlockPos(-1, -1, -1), BlockPos(1, 1, 1))) {
			if (Facing::choose(axis, offset.x, offset.y, offset.z) != 0)
				return neighbours;

			if (offset.distSqr(BlockPos::ZERO) != 2)
				return neighbours;

			neighbours.push_back(mPosition + offset);
		};
			
		return neighbours;
	}

	/**
	 * Specify whether this component can propagate speed to the other in any
	 * circumstance. Shaft and cogwheel connections are already handled by internal
	 * logic. Does not have to be specified on both ends, it is assumed that this
	 * relation is symmetrical.
	 *
	 * @param other
	 * @param state
	 * @param otherState
	 * @return true if this and the other component should check their propagation
	 * factor and are not already connected via integrated cogs or shafts
	 */
	bool isCustomConnection(KineticBlockEntity& other, const Block& state, const Block& otherState) {
		return false;
	}

	virtual bool canPropagateDiagonally(IRotate& block, const Block& state) {
		// return ICogWheel.isSmallCog(state);
		return false;
	}

	// @Override
	// public void requestModelDataUpdate() {
	// 	super.requestModelDataUpdate();
	// 	if (!this.remove)
	// 		CatnipServices.PLATFORM.executeOnClientOnly(() -> () -> VisualizationHelper.queueUpdate(this));
	// }

	bool isNoisy() {
		return true;
	}

	int getRotationAngleOffset(Facing::Axis axis) const {
		return 0;
	}

	bool syncSequenceContext() {
		return false;
	}

	// port helpers
	static bool isKineticBlockEntity(const BlockActor& be) {
		return be.getType() == KineticBlockEntity::TYPE;
	} 

	static KineticBlockEntity* asKineticBlockEntity(BlockActor* be) {
		if (be == nullptr || !isKineticBlockEntity(*be))
			return nullptr;
		return static_cast<KineticBlockEntity*>(be);
	}

	static const KineticBlockEntity* asKineticBlockEntity(const BlockActor* be) {
		if (be == nullptr || !isKineticBlockEntity(*be))
			return nullptr;
		return static_cast<const KineticBlockEntity*>(be);
	}
};