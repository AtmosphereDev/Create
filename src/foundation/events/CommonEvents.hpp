#pragma once
#include <amethyst/runtime/events/GameEvents.hpp>
#include <mc/src/common/world/level/Level.hpp>
#include "foundation/utility/ServerSpeedProvider.hpp"

class CommonEvents {
public:
    static void onAfterTick(AfterTickEvent& ev) {
        if (!ev.mLevel.isClientSide) return;
        onServerTick(ev.mLevel);
    }

    static void onServerTick(Level& level) {
        ServerSpeedProvider::serverTick(level);
    }

    static void AddEventListeners() {
        auto& bus = Amethyst::GetEventBus();
        bus.AddListener<AfterTickEvent>(&onAfterTick);
    }
};