#include "TorquePropagator.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"

std::unordered_map<DimensionType, std::unordered_map<uint64_t, KineticNetwork>> TorquePropagator::networks;

KineticNetwork *TorquePropagator::getOrCreateNetworkFor(KineticBlockEntity *be)
{
    if (!be || be->network == std::nullopt)
        return nullptr;

    auto& map = networks[be->level->mId];

    auto it = map.find(be->network.value());
    if (it != map.end()) {
        return &it->second;
    }

    KineticNetwork newNetwork;
    newNetwork.id = be->network.value();

    auto [insertedIt, inserted] = map.emplace(be->network.value(), std::move(newNetwork));
    return &insertedIt->second;
}