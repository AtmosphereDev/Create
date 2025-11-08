#pragma once
#include "content/kinetics/TorquePropagator.hpp"
#include "AllBlocks.hpp"
#include "AllPartialModels.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsInputHandler.hpp"

class Create {
public:
    static TorquePropagator TORQUE_PROPAGATOR;

    static void Initialize() {
        AllBlocks::AddEventListeners();
        AllPartialModels::AddEventListeners();

        // Misc event handlers
        ValueSettingsInputHandler::RegisterEvents();
    }
};