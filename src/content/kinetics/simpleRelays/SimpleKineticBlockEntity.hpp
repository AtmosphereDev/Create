#pragma once
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include "content/kinetics/simpleRelays/ICogWheel.hpp"

class SimpleKineticBlockEntity : public KineticBlockEntity {
public:
    SimpleKineticBlockEntity(const BlockPos& pos, const std::string& id)
        : KineticBlockEntity(pos, id) {}

    virtual std::vector<BlockPos> addPropagationLocations(IRotate& block, const Block& state, std::vector<BlockPos> neighbours) override {
        if (ICogWheel::isLargeCog(state)) {
            return KineticBlockEntity::addPropagationLocations(block, state, neighbours);
        }
        
        for (const auto& offset : BlockPos::betweenClosed(BlockPos(-1, -1, -1), BlockPos(1, 1, 1))) {
            if (offset.distSqr(BlockPos::ZERO) == 2) {
                neighbours.push_back(this->getBlockPos() + offset);
            }
        }

        return neighbours;
    }

    virtual bool isNoisy() const override {
        return false;
    }
};