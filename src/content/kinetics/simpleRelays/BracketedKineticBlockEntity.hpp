#pragma once
#include "content/kinetics/simpleRelays/SimpleKineticBlockEntity.hpp"

class BracketedKineticBlockEntity : public SimpleKineticBlockEntity /*, TransformableBlockEntity */ {
public:
    BracketedKineticBlockEntity(const BlockPos& pos, const std::string& id)
        : SimpleKineticBlockEntity(pos, id) {}

    virtual void addBehaviours(std::vector<std::shared_ptr<BlockEntityBehaviour>>& behavioursList) override {
        // behaviours
		// 	.add(new BracketedBlockEntityBehaviour(this, state -> state.getBlock() instanceof AbstractSimpleShaftBlock));
        SimpleKineticBlockEntity::addBehaviours(behavioursList);
    }
};