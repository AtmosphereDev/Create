#include "ClientInputs.hpp"
#include <amethyst/runtime/events/InputEvents.hpp>
#include <amethyst/runtime/ModContext.hpp>

bool ClientInputs::shiftKeyDown = false;
bool ClientInputs::useKeyDown = false;
short ClientInputs::mouseX = 0;
short ClientInputs::mouseY = 0;

void ClientInputs::AddEventListeners()
{
    auto& bus = Amethyst::GetEventBus();

    bus.AddListener<MouseInputEvent>([](MouseInputEvent& ev) -> void {
        // 0 is move
        // action button id 1 is left click
        if (ev.mActionButtonId == 0) {
            mouseX = ev.x;
            mouseY = ev.y;
            return;
        }

        if (ev.mActionButtonId == 2) {
            useKeyDown = ev.mButtonData != 0;
        }
    });
}