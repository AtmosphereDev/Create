#pragma once
#include "content/kinetics/TorquePropagator.hpp"
#include "AllBlocks.hpp"
#include "AllPartialModels.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsInputHandler.hpp"
#include "foundation/events/ClientEvents.hpp"
#include "porting/ClientInputs.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsScreen.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsClient.hpp"
#include "AllPackets.hpp"

class Create {
public:
    static TorquePropagator TORQUE_PROPAGATOR;

    static void Initialize() {
        AllBlocks::AddEventListeners();
        AllPartialModels::AddEventListeners();
        ClientEvents::AddEventListeners();
        ClientInputs::AddEventListeners();

        // Misc implementation event handlers
        ValueSettingsScreen::AddEventListeners();
        ValueSettingsClient::AddEventListeners();
        ValueSettingsInputHandler::RegisterEvents();

        // Misc other things
        AllPackets::RegisterPackets();
    }
};