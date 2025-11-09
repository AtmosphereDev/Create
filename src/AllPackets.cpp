#include "AllPackets.hpp"
#include <amethyst/runtime/ModContext.hpp>
#include <amethyst/runtime/networking/NetworkManager.hpp>
#include "foundation/blockEntity/behaviour/ValueSettingsPacket.hpp"
#include "foundation/networking/BlockEntityConfigurationPacket.hpp"

void AllPackets::RegisterPackets()
{
    auto& networking = Amethyst::GetNetworkManager();
    networking.RegisterPacketType<ValueSettingsPacket>(std::make_unique<BlockEntityConfigurationPacketHandler>());
}