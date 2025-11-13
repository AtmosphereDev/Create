#include "AllItems.hpp"
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src/common/world/item/registry/ItemRegistry.hpp>
#include "content/kinetics/belt/item/BeltConnectorItem.hpp"

static WeakPtr<BeltConnectorItem> BELT = nullptr;

void AllItems::RegisterItems(RegisterItemsEvent &ev)
{
    ev.itemRegistry.registerItemShared<BeltConnectorItem>("fx_create:belt_connector", ev.itemRegistry.getNextItemID());
}

void AllItems::AddEventListeners()
{
    auto& bus = Amethyst::GetEventBus();
    bus.AddListener<RegisterItemsEvent>(AllItems::RegisterItems);
}
