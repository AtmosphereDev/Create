#pragma once
#include "content/kinetics/base/RotatedPillarKineticBlock.hpp"

class AbstractShaftBlock : public RotatedPillarKineticBlock {
public:
    AbstractShaftBlock(const std::string& name, short id, const Material& material)
        : RotatedPillarKineticBlock(name, id, material) {}

    virtual Facing::Axis getRotationAxis(const Block& state) const override {
        return state.getState<Facing::Axis>(VanillaStates::PillarAxis);
    }

    virtual bool hasShaftTowards(const Dimension& world, const BlockPos& pos, const Block& state, FacingID face) const override {
        return Facing::getAxis(face) == state.getState<Facing::Axis>(VanillaStates::PillarAxis); 
    }
};