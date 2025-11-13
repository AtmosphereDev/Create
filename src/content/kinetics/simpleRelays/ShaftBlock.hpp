#pragma once
#include "content/kinetics/simpleRelays/AbstractSimpleShaftBlock.hpp"
#include "content/kinetics/simpleRelays/BracketedKineticBlockEntity.hpp"
#include "AllBlocks.hpp"

class ShaftBlock : public AbstractSimpleShaftBlock {
public:
    ShaftBlock(const std::string& name, short id, const Material& material)
        : AbstractSimpleShaftBlock(name, id, material) {}

    virtual std::shared_ptr<BlockActor> newBlockEntity(const BlockPos& pos, const Block& block) const override {
		return std::make_shared<BracketedKineticBlockEntity>(pos, "ShaftBlock");
	}

    static bool isShaft(const Block& block) {
        return AllBlocks::SHAFT == block.mLegacyBlock;
    }
};