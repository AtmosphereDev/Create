#pragma once
#include "porting/JavaBlockEntity.hpp"

class Dimension;

class SynchedBlockEntity : public JavaBlockEntity {
public:
	SynchedBlockEntity(const BlockPos& pos, const std::string& id)
		: JavaBlockEntity(pos, id) {}
	

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

	void sendData() {
		if (level->isClientSide()) return;
		level->mBlockSource->fireBlockChanged(mPosition, 0, getBlock(), getBlock(), 3, BlockChangedEventTarget::SelfBlock, nullptr, nullptr);
	}

	void notifyUpdate() {
		setChanged();
		sendData();
	}

	// public HolderGetter<Block> blockHolderGetter() {
	// 	return level != null ? level.holderLookup(Registries.BLOCK) : BuiltInRegistries.BLOCK.asLookup();
	// }
};