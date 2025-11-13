#pragma once
#include <amethyst/runtime/events/RegisterEvents.hpp>
#include <mc/src/common/SharedPtr.hpp>

class BeltConnectorItem;

class AllItems {
public:
    WeakPtr<BeltConnectorItem> BELT_CONNECTOR;

    static void RegisterItems(RegisterItemsEvent& ev);
    static void AddEventListeners();
};