#pragma once
#include "foundation/networking/BlockEntityConfigurationPacket.hpp"
#include "foundation/blockEntity/SmartBlockEntity.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsBehaviour.hpp"
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"

class ValueSettingsPacket : public BlockEntityConfigurationPacket {
public:
    int row;
    int value;
    FacingID side;
    bool ctrlDown;
    int behaviourIndex;
    // HitResult hitResult;

    ValueSettingsPacket(const BlockPos& pos,int row, int value, FacingID side, bool ctrlDown, int behaviourIndex)
        : BlockEntityConfigurationPacket(pos), row(row), value(value), side(side), ctrlDown(ctrlDown), behaviourIndex(behaviourIndex) {}

    ValueSettingsPacket() = default;

    virtual std::string getName() const override {
        return "ValueSettingsPacket";
    }

    virtual void write(BinaryStream& out) override {
        BlockEntityConfigurationPacket::write(out);
        out.write(row);
        out.write(value);
        out.write(static_cast<unsigned char>(side));
        out.write(ctrlDown);
        out.write(behaviourIndex);
    }

    virtual Bedrock::Result<void, std::error_code> read(ReadOnlyBinaryStream& in) override {
        BlockEntityConfigurationPacket::read(in);
        row = in.get<int>().value();
        value = in.get<int>().value();
        side = static_cast<FacingID>(in.get<unsigned char>().value());
        ctrlDown = in.get<bool>().value();
        behaviourIndex = in.get<int>().value();
        return {};
    }

    virtual void applySettings(ServerPlayer& player, SmartBlockEntity& be) const override {
        for (auto& [type, behaviourShared] : be.getAllBehaviours()) {
            BlockEntityBehaviour* behaviourPtr = behaviourShared.get();
            ValueSettingsBehaviour* valueSettingsBehaviour = dynamic_cast<ValueSettingsBehaviour*>(behaviourPtr);
            if (!valueSettingsBehaviour) 
                continue;
            if (!valueSettingsBehaviour->acceptsValueSettings())
                continue;
            if (behaviourIndex != valueSettingsBehaviour->netId()) 
                continue;

            auto settings = std::make_shared<ValueSettingsBehaviour::ValueSettings>(row, value);
            valueSettingsBehaviour->setValueSettings(player, settings, ctrlDown);
        }
    }
};