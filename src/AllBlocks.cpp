#include "AllBlocks.hpp"
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src/common/world/level/block/registry/BlockTypeRegistry.hpp>
#include <mc/src/common/world/level/block/BlockLegacy.hpp>
#include <mc/src/common/world/level/block/definition/BlockDefinitionGroup.hpp>
#include <mc/src/common/world/item/BlockItem.hpp>
#include <mc/src/common/world/item/registry/ItemRegistry.hpp>
#include <mc/src-client/common/client/renderer/block/BlockGraphics.hpp>
#include <mc/src/common/world/level/block/VanillaStates.hpp>

WeakPtr<RotatedPillarKineticBlock> AllBlocks::SHAFT;

void AllBlocks::RegisterBlocks(RegisterBlocksEvent &ev)
{
    Material& material = Material::getMaterial(MaterialType::Wood);
    SHAFT = BlockTypeRegistry::registerBlock<RotatedPillarKineticBlock>("fx_create:shaft", ev.blockDefinitions.getNextBlockId(), material);
    SHAFT->addState(VanillaStates::PillarAxis);
}

void AllBlocks::RegisterBlockItems(RegisterItemsEvent &ev)
{
    ev.itemRegistry.registerItemShared<BlockItem>(SHAFT->mNameInfo.mFullName.getString(), ev.itemRegistry.getNextItemID(), SHAFT->mNameInfo.mFullName);
}

void AllBlocks::InitBlockGraphics(InitBlockGraphicsEvent &ev)
{

}

void AllBlocks::AddEventListeners()
{
    auto& bus = Amethyst::GetEventBus();
    bus.AddListener<RegisterBlocksEvent>(&AllBlocks::RegisterBlocks);
    bus.AddListener<RegisterItemsEvent>(&AllBlocks::RegisterBlockItems);
    bus.AddListener<InitBlockGraphicsEvent>(&AllBlocks::InitBlockGraphics);
}
