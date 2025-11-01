#pragma once
#include <amethyst/runtime/events/RegisterEvents.hpp>
#include "content/kinetics/base/RotatedPillarKineticBlock.hpp"

class AllBlocks {
public:
    static WeakPtr<RotatedPillarKineticBlock> SHAFT;

    static void RegisterBlocks(RegisterBlocksEvent& ev);
    static void RegisterBlockItems(RegisterItemsEvent& ev);
    static void InitBlockGraphics(InitBlockGraphicsEvent& ev);

    static void AddEventListeners();
};