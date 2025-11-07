#pragma once
#include "content/kinetics/base/IRotate.hpp"

class KineticBlock : public IRotate {
public:
    KineticBlock(const std::string& name, short id, const Material& material)
        : IRotate(name, id, material) {}

    virtual void onRemove(BlockSource& blockSource, const BlockPos& pos) const override;

    virtual bool hasShaftTowards(const Dimension& world, const BlockPos& pos, const Block& state, FacingID face) const override {
        return false;
    }

    // equivalent of updateIndirectNeighbourShapes (i hope, not entirely sure)
    virtual void neighborChanged(BlockSource& unk0, const BlockPos& unk1, const BlockPos& unk2) const override;
};