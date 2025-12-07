#pragma once
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"

class DeferralBehaviour : public BlockEntityBehaviour {
public:
    static const BehaviourType TYPE;

private:
    bool needsUpdate;
    std::function<bool()> callback;

public:
    DeferralBehaviour(std::shared_ptr<SmartBlockEntity> be, const std::function<bool()>& callback)
        : BlockEntityBehaviour(be), needsUpdate(false), callback(callback) {}
    
    virtual bool isSafeNBT() override {
        return true;
    }

    virtual void write(CompoundTag& nbt, bool clientPacket) override {
        nbt.putByte("NeedsUpdate", needsUpdate);
        BlockEntityBehaviour::write(nbt, clientPacket);
    }

    virtual void read(const CompoundTag& nbt, bool clientPacket) override {
        needsUpdate = nbt.getByte("NeedsUpdate") != 0;
        BlockEntityBehaviour::read(nbt, clientPacket);
    }

    virtual void tick() override {
        BlockEntityBehaviour::tick();

        if (needsUpdate && callback())
            needsUpdate = false;
    }

    void scheduleUpdate() {
        needsUpdate = true;
    }

    virtual const BehaviourType& getType() const override {
        return TYPE;
    }
};