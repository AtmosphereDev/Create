#pragma once
#include <mc/src/common/world/level/block/actor/BlockActor.hpp>

class SynchedBlockEntity : public BlockActor {
	// public SyncedBlockEntity(BlockEntityType<?> type, BlockPos pos, BlockState state) {
	// 	super(type, pos, state);
	// }

	// @Override
	// public CompoundTag getUpdateTag(HolderLookup.Provider registries) {
	// 	return writeClient(new CompoundTag(), registries);
	// }

	// @Override
	// public ClientboundBlockEntityDataPacket getUpdatePacket() {
	// 	return ClientboundBlockEntityDataPacket.create(this);
	// }

	// @Override
	// public void handleUpdateTag(CompoundTag tag, HolderLookup.Provider registries) {
	// 	readClient(tag, registries);
	// }

	// @Override
	// public void onDataPacket(Connection net, ClientboundBlockEntityDataPacket pkt, HolderLookup.Provider registries) {
	// 	CompoundTag tag = pkt.getTag();
	// 	readClient(tag == null ? new CompoundTag() : tag, registries);
	// }

	// // Special handling for client update packets
	// public void readClient(CompoundTag tag, HolderLookup.Provider registries) {
	// 	loadAdditional(tag, registries);
	// }

	// // Special handling for client update packets
	// public CompoundTag writeClient(CompoundTag tag, HolderLookup.Provider registries) {
	// 	saveAdditional(tag, registries);
	// 	return tag;
	// }

	// public void sendData() {
	// 	if (level instanceof ServerLevel serverLevel)
	// 		serverLevel.getChunkSource().blockChanged(getBlockPos());
	// }

	// public void notifyUpdate() {
	// 	setChanged();
	// 	sendData();
	// }

	// public HolderGetter<Block> blockHolderGetter() {
	// 	return level != null ? level.holderLookup(Registries.BLOCK) : BuiltInRegistries.BLOCK.asLookup();
	// }
}