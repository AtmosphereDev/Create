#pragma once
#include <amethyst/runtime/events/RegisterEvents.hpp>
#include <mc/src/common/SharedPtr.hpp>
#include <mc/src/common/world/Facing.hpp>

class ShaftBlock;
class GearboxBlock;
class CreativeMotorBlock;
class CogWheelBlock;
class BeltBlock;

class AllBlocks {
public:
    static WeakPtr<ShaftBlock> SHAFT;
    static WeakPtr<GearboxBlock> GEARBOX;
    static WeakPtr<CreativeMotorBlock> CREATIVE_MOTOR;
    static WeakPtr<CogWheelBlock> COGWHEEL;
    static WeakPtr<CogWheelBlock> LARGE_COGWHEEL;
    static WeakPtr<BeltBlock> BELT;

    static void RegisterBlocks(RegisterBlocksEvent& ev);
    static void RegisterBlockItems(RegisterItemsEvent& ev);
    static void InitBlockGraphics(InitBlockGraphicsEvent& ev);

    static void AddEventListeners();
};