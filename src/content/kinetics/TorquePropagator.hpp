#pragma once
#include <amethyst/Imports.hpp>
#include <mc/src-deps/core/utility/AutomaticID.hpp>
#include "src/content/kinetics/base/KineticBlockEntity.hpp"
#include <mc/src/common/world/level/dimension/Dimension.hpp>

class TorquePropagator {
public:
    static std::unordered_map<DimensionType, std::unordered_map<uint64_t, KineticNetwork>> networks;

    // these should be "onUnloadDimension" on bedrock

	// void onLoadWorld(LevelAccessor world) {
	// 	networks.put(world, new HashMap<>());
	// 	Create.LOGGER.debug("Prepared Kinetic Network Space for " + WorldHelper.getDimensionID(world));
	// }

	// void onUnloadWorld(LevelAccessor world) {
	// 	networks.remove(world);
	// 	Create.LOGGER.debug("Removed Kinetic Network Space for " + WorldHelper.getDimensionID(world));
	// }

    KineticNetwork* getOrCreateNetworkFor(KineticBlockEntity* be) {
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
};