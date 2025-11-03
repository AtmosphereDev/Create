#pragma once
#include "content/kinetics/simpleRelays/AbstractSimpleShaftBlock.hpp"

class ShaftBlock : public AbstractSimpleShaftBlock {
public:
    ShaftBlock(const std::string& name, short id, const Material& material)
        : AbstractSimpleShaftBlock(name, id, material) {}

    virtual std::shared_ptr<BlockActor> newBlockEntity(const BlockPos& pos, const Block& block) const override {
		return std::make_shared<KineticBlockEntity>(JavaBlockEntity::TYPE, pos, "bosh");
	}
};