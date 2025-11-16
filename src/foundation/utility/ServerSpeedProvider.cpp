#include "ServerSpeedProvider.hpp"

LerpedFloat ServerSpeedProvider::modifier = LerpedFloat::linear();
int ServerSpeedProvider::clientTimer = 0;
int ServerSpeedProvider::serverTimer = 0;
bool ServerSpeedProvider::initialized = false;