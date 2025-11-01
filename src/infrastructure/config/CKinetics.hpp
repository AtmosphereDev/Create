#pragma once
#include "infrastructure/config/ConfigBase.hpp"

class CKinetics {
public:
    ConfigInt maxRotationSpeed = ConfigInt(256, 64, "maxRotationSpeed");
};