#pragma once
#include "infrastructure/config/ConfigBase.hpp"

class CKinetics {
public:
	ConfigInt kineticValidationFrequency = ConfigInt(60, 5, "kineticValidationFrequency");
    ConfigInt maxRotationSpeed = ConfigInt(256, 64, "maxRotationSpeed");
    ConfigFloat mediumSpeed = ConfigFloat(30.0f, 0.0f, 4096.0f, "mediumSpeed");
    ConfigFloat fastSpeed = ConfigFloat(100.0f, 0.0f, 65535.0f, "fastSpeed");
    ConfigInt maxBeltLength = ConfigInt(20, 5, "maxBeltLength");
};