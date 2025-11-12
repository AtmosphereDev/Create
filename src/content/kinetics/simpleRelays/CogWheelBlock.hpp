#pragma once
#include "content/kinetics/simpleRelays/AbstractSimpleShaftBlock.hpp"
#include "content/kinetics/simpleRelays/ICogWheel.hpp"
#include "content/kinetics/simpleRelays/SimpleKineticBlockEntity.hpp"
#include "content/kinetics/simpleRelays/BracketedKineticBlockEntity.hpp"

class CogWheelBlock : public AbstractSimpleShaftBlock, public ICogWheel {
public:
    bool isLarge;

    CogWheelBlock(const std::string& name, short id, const Material& material, bool large)
        : AbstractSimpleShaftBlock(name, id, material), isLarge(large) {
            mTags.push_back(HashedString("ICogWheel"));
        }

    virtual bool isLargeCog() const override {
        return isLarge;
    }

    virtual bool isSmallCog() const override {
        return !isLarge;
    }

    virtual bool canSurvive(BlockSource& region, const BlockPos& pos) const override {
        auto& block = region.getBlock(pos);
        return isValidCogWheelPosition(ICogWheel::isLargeCog(block), *region.mDimension, pos, block.getState<Facing::Axis>(VanillaStates::PillarAxis));
    }

    virtual std::shared_ptr<BlockActor> newBlockEntity(const BlockPos& pos, const Block& block) const override {
		return std::make_shared<BracketedKineticBlockEntity>(pos, "A");
	}

    static bool isValidCogWheelPosition(bool large, const Dimension& dim, const BlockPos& pos, Facing::Axis cogAxis) {
        for (FacingID facing : Facing::DIRECTIONS) {
            if (Facing::getAxis(facing) == cogAxis)
                continue;

            BlockPos offsetPos = pos.neighbor(facing);
            const Block& blockState = dim.mBlockSource->getBlock(offsetPos);
            if (blockState.hasState(VanillaStates::PillarAxis) && Facing::getAxis(facing) == blockState.getState<Facing::Axis>(VanillaStates::PillarAxis)) 
                continue;

            if (ICogWheel::isLargeCog(blockState) || large && ICogWheel::isSmallCog(blockState)) {
                return false;
            }
        }
        return true;
    }
};