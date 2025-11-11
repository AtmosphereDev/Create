#include "AllBlocks.hpp"
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src/common/world/level/block/registry/BlockTypeRegistry.hpp>
#include <mc/src/common/world/level/block/BlockLegacy.hpp>
#include <mc/src/common/world/level/block/definition/BlockDefinitionGroup.hpp>
#include <mc/src/common/world/item/BlockItem.hpp>
#include <mc/src/common/world/item/registry/ItemRegistry.hpp>
#include <mc/src-client/common/client/renderer/block/BlockGraphics.hpp>
#include <mc/src/common/world/level/block/VanillaStates.hpp>

#include "content/kinetics/gearbox/GearboxBlock.hpp"
#include "content/kinetics/motor/CreativeMotorBlock.hpp"
#include "content/kinetics/simpleRelays/ShaftBlock.hpp"
#include "content/kinetics/simpleRelays/CogWheelBlock.hpp"

WeakPtr<ShaftBlock> AllBlocks::SHAFT;
WeakPtr<GearboxBlock> AllBlocks::GEARBOX;
WeakPtr<CreativeMotorBlock> AllBlocks::CREATIVE_MOTOR;
WeakPtr<CogWheelBlock> AllBlocks::COGWHEEL;
WeakPtr<CogWheelBlock> AllBlocks::LARGE_COGWHEEL;

void AllBlocks::RegisterBlocks(RegisterBlocksEvent &ev)
{
    Material& material = Material::getMaterial(MaterialType::NonSolid);
    SHAFT = BlockTypeRegistry::registerBlock<ShaftBlock>("fx_create:shaft", ev.blockDefinitions.getNextBlockId(), material);
    SHAFT->addState(VanillaStates::PillarAxis);

    GEARBOX = BlockTypeRegistry::registerBlock<GearboxBlock>("fx_create:gearbox", ev.blockDefinitions.getNextBlockId(), material);
	GEARBOX->addState(VanillaStates::PillarAxis);

    CREATIVE_MOTOR = BlockTypeRegistry::registerBlock<CreativeMotorBlock>("fx_create:creative_motor", ev.blockDefinitions.getNextBlockId(), material);
    CREATIVE_MOTOR->addState(VanillaStates::FacingDirection);

    COGWHEEL = BlockTypeRegistry::registerBlock<CogWheelBlock>("fx_create:cogwheel", ev.blockDefinitions.getNextBlockId(), material, false);
    COGWHEEL->addState(VanillaStates::PillarAxis);

    LARGE_COGWHEEL = BlockTypeRegistry::registerBlock<CogWheelBlock>("fx_create:large_cogwheel", ev.blockDefinitions.getNextBlockId(), material, true);
    LARGE_COGWHEEL->addState(VanillaStates::PillarAxis);
}

void AllBlocks::RegisterBlockItems(RegisterItemsEvent &ev)
{
    ev.itemRegistry.registerItemShared<BlockItem>(SHAFT->mNameInfo.mFullName.getString(), ev.itemRegistry.getNextItemID(), SHAFT->mNameInfo.mFullName);
    ev.itemRegistry.registerItemShared<BlockItem>(GEARBOX->mNameInfo.mFullName.getString(), ev.itemRegistry.getNextItemID(), GEARBOX->mNameInfo.mFullName);
    ev.itemRegistry.registerItemShared<BlockItem>(CREATIVE_MOTOR->mNameInfo.mFullName.getString(), ev.itemRegistry.getNextItemID(), CREATIVE_MOTOR->mNameInfo.mFullName);
    ev.itemRegistry.registerItemShared<BlockItem>(COGWHEEL->mNameInfo.mFullName.getString(), ev.itemRegistry.getNextItemID(), COGWHEEL->mNameInfo.mFullName);
    ev.itemRegistry.registerItemShared<BlockItem>(LARGE_COGWHEEL->mNameInfo.mFullName.getString(), ev.itemRegistry.getNextItemID(), LARGE_COGWHEEL->mNameInfo.mFullName);

    auto& construction = ev.mCreativeItemRegistry.GetVanillaCategory(CreativeItemCategory::Construction);
    construction.AddCreativeItem(SHAFT->getRenderBlock());
    construction.AddCreativeItem(GEARBOX->getRenderBlock());
    construction.AddCreativeItem(CREATIVE_MOTOR->getRenderBlock());
    construction.AddCreativeItem(COGWHEEL->getRenderBlock());
    construction.AddCreativeItem(LARGE_COGWHEEL->getRenderBlock());
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
    createGraphics(CREATIVE_MOTOR);
    createGraphics(COGWHEEL);
    createGraphics(LARGE_COGWHEEL);
}

void AllBlocks::AddEventListeners()
{
    auto& bus = Amethyst::GetEventBus();
    bus.AddListener<RegisterBlocksEvent>(&AllBlocks::RegisterBlocks);
    bus.AddListener<RegisterItemsEvent>(&AllBlocks::RegisterBlockItems);
    bus.AddListener<InitBlockGraphicsEvent>(&AllBlocks::InitBlockGraphics);
    
    // Misc hooks just for porting
    JavaBlockEntityHooks();
}
