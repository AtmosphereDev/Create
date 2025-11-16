#pragma once
#include <cstdint>
#include "infrastructure/config/AllConfigs.hpp"
#include "catnip/animation/LerpedFloat.hpp"
#include "amethyst/runtime/networking/CustomPacket.hpp"
#include "amethyst/runtime/ModContext.hpp"
#include "mc/src/common/Minecraft.hpp"

class ServerSpeedProvider {
private:
    static LerpedFloat modifier;
    static int clientTimer;
    static int serverTimer;
    static bool initialized;

public:
    static void serverTick(Level& level) {
        serverTimer++;

        if (serverTimer >= getSyncInterval()) {
            serverTimer = 0;
            
            auto& networking = Amethyst::GetNetworkManager();
            networking.SendBroadcast(
                Amethyst::GetServerCtx().mMinecraft->mPacketSender,
                std::make_unique<SpeedServerProviderPacket>()
            );
        }
    }

    static void clientTick() {
		static bool toggle = false;
        if (toggle) {
            toggle = false;
            return;
		}

		toggle = true;

        // todo check if paused?
        modifier.tickChaser();
        clientTimer++;
    }

    static int getSyncInterval() {
        return AllConfigs::server().tickrateSyncTimer.get();
    }

    static float get() {
        return modifier.getValue();
    }

    class SpeedServerProviderPacket : public Amethyst::CustomPacket {
    public:
        virtual std::string getName() const {
            return "SpeedServerProviderPacket";
        };

        virtual void write(BinaryStream& out) {}
        virtual Bedrock::Result<void, std::error_code> read(ReadOnlyBinaryStream& in) {
            return {};
        }
    };

    class SpeedServerProviderPacketHandler : public Amethyst::CustomPacketHandler {
    public:
        virtual void handle(const NetworkIdentifier& networkId, NetEventCallback& callback, const Amethyst::CustomPacket& _packet) const override {
            if (!initialized) {
                initialized = true;
                clientTimer = 0;
                return;
            }

            float target = ((float)getSyncInterval()) / std::max(clientTimer, 1);
            modifier.chase(std::min(target, 1.0f), 0.25f, LerpedFloat::ChaserExp());
            // Set this to -1 because packets are processed before ticks.
			// ServerSpeedProvider#clientTick will increment it to 0 at the end of this tick.
			// Setting it to 0 causes consistent desync, as the client ends up counting too many ticks.
            clientTimer = -1;

            // todo know if this applies to bedrock^^
        }
    };

    static void AddEventListeners() {
        auto& networking = Amethyst::GetNetworkManager();
        networking.RegisterPacketType<SpeedServerProviderPacket>(std::make_unique<SpeedServerProviderPacketHandler>());
    }
};