#pragma once
#include <amethyst/Imports.hpp>
#include <mc/src-deps/core/utility/AutomaticID.hpp>
#include <mc/src/common/world/level/dimension/Dimension.hpp>

class KineticBlockEntity;
class KineticNetwork;

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

    KineticNetwork* getOrCreateNetworkFor(KineticBlockEntity* be);
};