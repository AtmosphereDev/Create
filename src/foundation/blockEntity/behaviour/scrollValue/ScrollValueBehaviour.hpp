#pragma once
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"
#include "foundation/blockEntity/behaviour/ValueBoxTransform.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsBehaviour.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"

class ScrollValueBehaviour : public BlockEntityBehaviour, public ValueSettingsBehaviour {
public:
	static const BehaviourType TYPE;

	std::shared_ptr<ValueBoxTransform> slotPositioning;
	Vec3 textShift;

	int min = 0;
	int max = 1;
	int value;
	std::string label;
	std::function<void(int)> callback;
	std::function<void(int)> clientCallback;
	std::function<std::string(int)> formatter;
	std::function<bool()> isActiveCallback;
	bool needsWrench;

    ScrollValueBehaviour(std::string label, SmartBlockEntity* be, std::shared_ptr<ValueBoxTransform> slot) 
        : BlockEntityBehaviour(be)
    {
        setLabel(label);
		slotPositioning = slot;
		callback = [](int) {};
		clientCallback = [](int) {};
		formatter = [](int v) { return std::to_string(v); };
		value = 0;
		isActiveCallback = []() { return true; };
    }

	virtual bool isSafeNBT() override {
		return true;
	}

	virtual void write(CompoundTag& nbt, bool clientPacket) override {
		nbt.putInt("ScrollValue", value);
		BlockEntityBehaviour::write(nbt, clientPacket);
	}

	virtual void read(const CompoundTag& nbt, bool clientPacket) override {
		value = nbt.getInt("ScrollValue");
		BlockEntityBehaviour::read(nbt, clientPacket);
	}

	ScrollValueBehaviour& withClientCallback(const std::function<void(int)>& clientCallback) {
		this->clientCallback = clientCallback;
		return *this;
	}

	ScrollValueBehaviour& withCallback(const std::function<void(int)>& callback) {
		this->callback = callback;
		return *this;
	}

	ScrollValueBehaviour& between(int min, int max) {
		this->min = min;
		this->max = max;
		return *this;
	}

	ScrollValueBehaviour& requiresWrench() {
		this->needsWrench = true;
		return *this;
	}

	ScrollValueBehaviour& withFormatter(const std::function<std::string(int)>& formatter) {
		this->formatter = formatter;
		return *this;
	}

	ScrollValueBehaviour& onlyActiveWhen(const std::function<bool()>& isActive) {
		this->isActiveCallback = isActive;
		return *this;
	}

	void setValue(int value) {
		value = std::clamp(value, min, max);
		if (value == this->value)
			return;

		this->value = value;
		if (callback) callback(value);
		blockEntity->setChanged();
		blockEntity->sendData();
	}

    int getValue() {
		return value;
	}

	std::string formatValue() {
		return formatter(value);
	}

	virtual const BehaviourType& getType() const override {
		return TYPE;
	}

	virtual bool isActive() const override {
		return isActiveCallback();
	}

	virtual bool testHit(const Vec3& hit) {
		const Block& state = blockEntity->getBlock();
		Vec3 localHit = hit - Vec3::atLowerCornerOf(blockEntity->getBlockPos());
		return slotPositioning->testHit(*blockEntity->getLevel().mBlockSource, blockEntity->getBlockPos(), state, localHit);
	}

	void setLabel(const std::string& label) {
		this->label = label;
	}

	class StepContext {
	public:
		int currentValue;
		bool forward;
		bool shift;
		bool control;
	};

	virtual std::shared_ptr<ValueBoxTransform> getSlotPositioning() const override {
		return slotPositioning;
	}

	virtual ValueSettingsBoard createBoard(const Player& player, const HitResult& hit) override;

	virtual void setValueSettings(const Player& player, std::shared_ptr<ValueSettings> valueSetting, bool ctrlDown) override {
		if (valueSetting == getValueSettings()) 
			return;
		setValue(valueSetting->value);
		playFeedbackSound(*this);
	}

	virtual std::shared_ptr<ValueSettings> getValueSettings() override {
		return std::make_shared<ValueSettings>(0, value);
	}

	bool onlyVisibleWithWrench() const override {
		return needsWrench;
	}

	virtual void onShortInteract(const Player& player, FacingID side, const HitResult& hit) override {
		// if (player instanceof FakePlayer)
		// 	blockEntity.getBlockState()
		// 		.useItemOn(player.getItemInHand(hand), getWorld(), player, hand, hitResult);
	}
};