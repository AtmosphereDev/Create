#pragma once
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include "foundation/blockEntity/behaviour/simple/DeferralBehaviour.hpp"

class BasinOperatingBlockEntity : public KineticBlockEntity {
public:
    std::shared_ptr<DeferralBehaviour> basinChecker;
    bool basinRemoved;
    // currentRecipe

    BasinOperatingBlockEntity(const BlockPos& pos, const std::string& id)
        : KineticBlockEntity(pos, id), basinRemoved(false) {}

    virtual void addBehaviours(std::vector<std::shared_ptr<BlockEntityBehaviour>>& behavioursList) override {
        basinChecker = std::make_shared<DeferralBehaviour>(this->getShared(), std::bind(&BasinOperatingBlockEntity::updateBasin, this));
        behavioursList.push_back(basinChecker);
    }

protected:
    bool updateBasin() {
        return true; // temp
    }
};