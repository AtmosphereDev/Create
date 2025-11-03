#include "AllBlocks.hpp"
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src/common/world/level/block/registry/BlockTypeRegistry.hpp>
#include <mc/src/common/world/level/block/BlockLegacy.hpp>
#include <mc/src/common/world/level/block/definition/BlockDefinitionGroup.hpp>
#include <mc/src/common/world/item/BlockItem.hpp>
#include <mc/src/common/world/item/registry/ItemRegistry.hpp>
#include <mc/src-client/common/client/renderer/block/BlockGraphics.hpp>
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include "content/kinetics/base/RotatedPillarKineticBlock.hpp"
#include "content/kinetics/gearbox/GearboxBlock.hpp"
#include "content/kinetics/simpleRelays/ShaftBlock.hpp"

WeakPtr<ShaftBlock> AllBlocks::SHAFT;
WeakPtr<GearboxBlock> AllBlocks::GEARBOX;

void AllBlocks::RegisterBlocks(RegisterBlocksEvent &ev)
{
    Material& material = Material::getMaterial(MaterialType::Solid);
    SHAFT = BlockTypeRegistry::registerBlock<ShaftBlock>("fx_create:shaft", ev.blockDefinitions.getNextBlockId(), material);
    SHAFT->addState(VanillaStates::PillarAxis);

    GEARBOX = BlockTypeRegistry::registerBlock<GearboxBlock>("fx_create:gearbox", ev.blockDefinitions.getNextBlockId(), material);
	GEARBOX->addState(VanillaStates::PillarAxis);
}

void AllBlocks::RegisterBlockItems(RegisterItemsEvent &ev)
{
    ev.itemRegistry.registerItemShared<BlockItem>(SHAFT->mNameInfo.mFullName.getString(), ev.itemRegistry.getNextItemID(), SHAFT->mNameInfo.mFullName);
    ev.itemRegistry.registerItemShared<BlockItem>(GEARBOX->mNameInfo.mFullName.getString(), ev.itemRegistry.getNextItemID(), GEARBOX->mNameInfo.mFullName);
}

void AllBlocks::InitBlockGraphics(InitBlockGraphicsEvent &ev)
{
    // lambda func
    auto createGraphics = [&](const WeakPtr<BlockLegacy>& block) {
        BlockGraphics* graphics = BlockGraphics::createBlockGraphics(block->mNameInfo.mFullName, BlockShape::INVISIBLE);
        graphics->setTextureItem("missing");
        graphics->setDefaultCarriedTextures();
    };

    createGraphics(SHAFT);
    createGraphics(GEARBOX);
}

void AllBlocks::AddEventListeners()
{
    auto& bus = Amethyst::GetEventBus();
    bus.AddListener<RegisterBlocksEvent>(&AllBlocks::RegisterBlocks);
    bus.AddListener<RegisterItemsEvent>(&AllBlocks::RegisterBlockItems);
    bus.AddListener<InitBlockGraphicsEvent>(&AllBlocks::InitBlockGraphics);
}
