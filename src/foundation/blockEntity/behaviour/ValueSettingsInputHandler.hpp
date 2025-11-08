#pragma once
#include <amethyst/runtime/ModContext.hpp>
#include <amethyst/runtime/events/BlockEvents.hpp>

class Player;

class ValueSettingsInputHandler {
public:
    static void beforeBlockUse(BeforeBlockUseEvent& ev);

    static bool canInteract(const Player& player) {
        // todo check if holding shift
        // return player != null && !player.isSpectator() && !player.isShiftKeyDown() && !AdventureUtil.isAdventure(player);
        return true;
    }

    static void RegisterEvents() {
        auto& bus = Amethyst::GetEventBus();
        bus.AddListener<BeforeBlockUseEvent>(&ValueSettingsInputHandler::beforeBlockUse);
    }
};