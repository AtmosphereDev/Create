#include "TransportedItemStackHandlerBehaviour.hpp"
#include "content/kinetics/belt/transport/TransportedItemStack.hpp"

const BehaviourType TransportedItemStackHandlerBehaviour::TYPE = BehaviourType("TransportedItemStackHandlerBehaviour");

bool TransportedItemStackHandlerBehaviour::TransportedResult::didntChangeFrom(const ItemStack &stackBefore) const
{
    return doesNothing()
        || outputs.size() == 1 && outputs.at(0)->stack == stackBefore && !hasHeldOutput();
}