#include "KineticBlockEntity.hpp"
#include "Create.hpp"

KineticNetwork *KineticBlockEntity::getOrCreateNetwork()
{
    return Create::TORQUE_PROPAGATOR.getOrCreateNetworkFor(this);
}