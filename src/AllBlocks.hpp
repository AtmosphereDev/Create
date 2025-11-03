#pragma once
#include <amethyst/runtime/events/RegisterEvents.hpp>
#include <mc/src/common/SharedPtr.hpp>

class ShaftBlock;
class GearboxBlock;

class AllBlocks {
public:
    static WeakPtr<ShaftBlock> SHAFT;
    static WeakPtr<GearboxBlock> GEARBOX;

    static void RegisterBlocks(RegisterBlocksEvent& ev);
    static void RegisterBlockItems(RegisterItemsEvent& ev);
    static void InitBlockGraphics(InitBlockGraphicsEvent& ev);

    static void AddEventListeners();
};