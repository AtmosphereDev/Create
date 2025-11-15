#pragma once

class ItemStack;
class BeltBlockEntity;
class BlockPos;
class BlockSource;
class Vec3;

class BeltHelper {
public:
    static bool isItemUpright(const ItemStack& stack) {
        // return uprightCache.computeIfAbsent(
		// 	stack.getItem(),
		// 	item -> {
		// 		boolean isFluidHandler = stack.getCapability(Capabilities.FluidHandler.ITEM) != null;
		// 		boolean useUpright = AllItemTags.UPRIGHT_ON_BELT.matches(stack);
		// 		boolean forceDisableUpright = !AllItemTags.NOT_UPRIGHT_ON_BELT.matches(stack);

		// 		return (isFluidHandler || useUpright) && forceDisableUpright;
		// 	}
		// );

        return false;
    }

	static BlockPos getPositionForOffset(BeltBlockEntity* controller, int offset);

	static BeltBlockEntity* getSegmentBE(BlockSource& region, const BlockPos& pos);

	static Vec3 getVectorForOffset(BeltBlockEntity* controller, float offset);
};