#pragma once
#include "CKinetics.hpp"
#include "infrastructure/config/ConfigBase.hpp"

class CServer {
public:
    CKinetics kinetics;

    ConfigInt tickrateSyncTimer = ConfigInt(20, 5, "tickrateSyncTimer");
};