#pragma once
#include "content/kinetics/TorquePropagator.hpp"
#include "AllBlocks.hpp"

class Create {
public:
    static TorquePropagator TORQUE_PROPAGATOR;

    static void Initialize() {
        AllBlocks::AddEventListeners();
    }
};