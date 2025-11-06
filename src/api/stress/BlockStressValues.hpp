#pragma once

class Block;

class BlockStressValues {
public:
    // /**
	//  * Registry for suppliers of stress impacts. Determine the base impact at 1 RPM.
	//  */
	// public static final SimpleRegistry<Block, DoubleSupplier> IMPACTS = SimpleRegistry.create();
	// /**
	//  * Registry for suppliers of stress capacities. Determine the base capacity at 1 RPM.
	//  */
	// public static final SimpleRegistry<Block, DoubleSupplier> CAPACITIES = SimpleRegistry.create();
	// /**
	//  * Registry for generator RPM values. This is only used for tooltips; actual functionality is determined by the block.
	//  */
	// public static final SimpleRegistry<Block, GeneratedRpm> RPM = SimpleRegistry.create();

	static double getImpact(const Block& block) {
		return 0;
		// DoubleSupplier supplier = IMPACTS.get(block);
		// return supplier == null ? 0 : supplier.getAsDouble();
	}

	static double getCapacity(const Block& block) {
        return 0;
		// DoubleSupplier supplier = CAPACITIES.get(block);
		// return supplier == null ? 0 : supplier.getAsDouble();
	}

	// /**
	//  * Shortcut for when a generator always generates the same RPM.
	//  */
	// public static NonNullConsumer<Block> setGeneratorSpeed(int value) {
	// 	return block -> RPM.register(block, new GeneratedRpm(value, false));
	// }

	// /**
	//  * Utility for Registrate. Registers the given RPM generation info to blocks passed to the returned consumer.
	//  */
	// public static NonNullConsumer<Block> setGeneratorSpeed(int value, boolean mayGenerateLess) {
	// 	return block -> RPM.register(block, new GeneratedRpm(value, mayGenerateLess));
	// }

	// public record GeneratedRpm(int value, boolean mayGenerateLess) {
	// }

	// private BlockStressValues() {
	// 	throw new AssertionError("This class should not be instantiated");
	// }
};