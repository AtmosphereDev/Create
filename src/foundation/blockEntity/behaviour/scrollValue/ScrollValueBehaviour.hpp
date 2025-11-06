#pragma once
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"
#include "foundation/blockEntity/behaviour/ValueBoxTransform.hpp"

class ScrollValueBehaviour : public BlockEntityBehaviour {
public:
    int value;

	static const BehaviourType TYPE;

    ScrollValueBehaviour(std::string label, SmartBlockEntity* be, std::shared_ptr<ValueBoxTransform> slot) 
        : BlockEntityBehaviour(be)
    {
        // this.setLabel(label);
		// slotPositioning = slot;
		// callback = i -> {
		// };
		// clientCallback = i -> {
		// };
		// formatter = i -> Integer.toString(i);
		value = 0;
		// isActive = () -> true;
    }

    int getValue() {
		return value;
	}

	virtual const BehaviourType& getType() const override {
		return TYPE;
	}
};