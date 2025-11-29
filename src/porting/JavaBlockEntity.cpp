#include "JavaBlockEntity.hpp"

BlockActorType JavaBlockEntity::TYPE = (BlockActorType)((int)BlockActorType::Count + 1);

//Amethyst::InlineHook<decltype(&LevelChunk::_createBlockEntity)> _LevelChunk__createBlockEntity;
//SafetyHookInline _LevelChunk__createBlockEntity;
//
//std::shared_ptr<BlockActor>* LevelChunk__createBlockEntity(LevelChunk* self, std::shared_ptr<BlockActor>* ret, const BlockPos& pos, BlockSource* currentSource, const Block& current, const Block& old) {
//    _LevelChunk__createBlockEntity.call
//        <std::shared_ptr<BlockActor>*, LevelChunk*, std::shared_ptr<BlockActor>*, const BlockPos&, BlockSource*, const Block&, const Block&>
//        (self, ret, pos, currentSource, current, old);
//
//	BlockActor* blockEntity = ret->get();
//	if (blockEntity != nullptr) {
//        if (JavaBlockEntity::IsJavaBlockEntity(*blockEntity)) {
//			JavaBlockEntity& javaEntity = static_cast<JavaBlockEntity&>(*blockEntity);
//            javaEntity._initializeAfterBlockEntityConstructed(*self);
//        }
//    }
//
//    return ret;
//}

Amethyst::InlineHook<decltype(&LevelChunk::_placeBlockEntity)> _LevelChunk__placeBlockEntity;

void LevelChunk__placeBlockEntity(LevelChunk* self, std::shared_ptr<BlockActor> actor) {
    _LevelChunk__placeBlockEntity(self, actor);

    if (actor != nullptr && JavaBlockEntity::IsJavaBlockEntity(*actor)) {
        JavaBlockEntity& javaEntity = static_cast<JavaBlockEntity&>(*actor);
        javaEntity._initializeAfterBlockEntityConstructed(*self);
	}
}

void JavaBlockEntityHooks()
{
    auto& hooks = Amethyst::GetHookManager();
    HOOK(LevelChunk, _placeBlockEntity);
}