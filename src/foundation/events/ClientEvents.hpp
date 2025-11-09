#pragma once
#include <amethyst/runtime/events/GameEvents.hpp>
#include <mc/src/common/world/level/Level.hpp>
#include "CreateClient.hpp"
#include "foundation/blockEntity/behaviour/scrollValue/ScrollValueRenderer.hpp"

class ClientEvents {
public:
    static void onBeforeTick(BeforeTickEvent& ev) {
        if (!ev.mLevel.isClientSide) return;
        onTick(ev.mLevel, true);
    }

    static void onAfterTick(AfterTickEvent& ev) {
        if (!ev.mLevel.isClientSide) return;
        onTick(ev.mLevel, false);
    }

    static void onTick(Level& level, bool isPreEvent) {
        if (isPreEvent) {
            // LinkedControllerClientHandler.tick();
			// ControlsHandler.tick();
			// AirCurrent.Client.tickClientPlayerSounds();
			// return;
        }

        ScrollValueRenderer::tick();
        CreateClient::VALUE_SETTINGS_HANDLER.tick();

    }

    static void AddEventListeners() {
        auto& bus = Amethyst::GetEventBus();
        bus.AddListener<BeforeTickEvent>(&onBeforeTick);
        bus.AddListener<AfterTickEvent>(&onAfterTick);
    }
};