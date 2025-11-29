#pragma once
#include <amethyst/runtime/networking/CustomPacket.hpp>
#include <mc/src/common/server/ServerPlayer.hpp>
#include <mc/src/common/network/ServerNetworkHandler.hpp>
#include <mc/src/common/world/level/BlockSource.hpp>
#include "foundation/blockEntity/SmartBlockEntity.hpp"

class BlockEntityConfigurationPacket : public Amethyst::CustomPacket {
public:
    BlockPos pos;

    BlockEntityConfigurationPacket(const BlockPos& pos)
        : pos(pos) {}

    BlockEntityConfigurationPacket() = default;

    virtual void write(BinaryStream& out) override {
        out.write(pos.x);
        out.write(pos.y);
        out.write(pos.z);
    }

    virtual Bedrock::Result<void, std::error_code> read(ReadOnlyBinaryStream& in) override {
        pos.x = in.get<int>().value();
        pos.y = in.get<int>().value();
        pos.z = in.get<int>().value();
        return {};
    }

    int maxRange() const {
        return 20;
    }

    virtual bool causeUpdate() const {
        return true;
    }

    virtual void applySettings(ServerPlayer& player, SmartBlockEntity& be) const = 0;
};

class BlockEntityConfigurationPacketHandler : public Amethyst::CustomPacketHandler {
public:
    virtual void handle(const NetworkIdentifier& networkId, NetEventCallback& callback, const Amethyst::CustomPacket& _packet) const override {
        const BlockEntityConfigurationPacket& packet = static_cast<const BlockEntityConfigurationPacket&>(_packet);
        ServerNetworkHandler& serverNetwork = (ServerNetworkHandler&)callback;
        ServerPlayer* serverPlayer = serverNetwork._getServerPlayer(networkId, SubClientId::PrimaryClient);
        if (!serverPlayer) return;

        const BlockSource& region = serverPlayer->getDimensionBlockSourceConst();
        if (!region.areChunksFullyLoaded(packet.pos, 1)) return;

        if (packet.pos.distSqr(*serverPlayer->getPosition()) > packet.maxRange()) return;

        BlockActor* blockEntity = region.getBlockEntity(packet.pos);
        if (!blockEntity) return;

        SmartBlockEntity* smartBE = dynamic_cast<SmartBlockEntity*>(blockEntity);
        if (!smartBE) return;

        packet.applySettings(*serverPlayer, *smartBE);
        if (!packet.causeUpdate()) return;

        smartBE->sendData();
        smartBE->setChanged();
    };
};