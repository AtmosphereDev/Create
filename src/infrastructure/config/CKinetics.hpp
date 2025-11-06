#pragma once
#include "infrastructure/config/ConfigBase.hpp"

class CKinetics {
public:
    ConfigInt maxRotationSpeed = ConfigInt(256, 64, "maxRotationSpeed");
    ConfigFloat mediumSpeed = ConfigFloat(30.0f, 0.0f, 4096.0f, "mediumSpeed");
    ConfigFloat fastSpeed = ConfigFloat(100.0f, 0.0f, 65535.0f, "fastSpeed");
};