#pragma once
#include <amethyst/runtime/ModContext.hpp>
#include <amethyst/runtime/events/BlockEvents.hpp>
#include "foundation/blockEntity/SmartBlockEntity.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsBehaviour.hpp"
#include <mc/src/common/world/level/Level.hpp>

class ValueSettingsInputHandler {
public:
    static void beforeBlockUse(BeforeBlockUseEvent& ev) {
        const BlockPos& pos = ev.pos;
        const Block& block = ev.block;
        Player& player = ev.player;
        Dimension& dimension = player.getDimension();

        if (!canInteract(player))
            return;

        // if (AllBlocks.CLIPBOARD.isIn(player.getMainHandItem()))
		// 	return;

        SmartBlockEntity* sbe = SmartBlockEntity::AsSmartBlockEntity(dimension.mBlockSource->getBlockEntity(pos));
        if (!sbe) return;

        bool isClientSide = player.isClientSide();

        if (isClientSide) {
            Log::Info("Run CreateClient.VALUE_SETTINGS_HANDLER.cancelIfWarmupAlreadyStarted(event)");
        }

        if (ev.IsCanceled())
            return;

        const PlayerInventory& inventory = player.getSupplies();
        const ItemStack& mainHandItem = inventory.getSelectedItem();
        const HitResult& ray = player.getLevel()->getHitResult();

        for (auto& [_, behaviourShared] : sbe->getAllBehaviours()) {
            auto behaviour = behaviourShared.get();
            if (!behaviour) continue;

            auto* valueSettingsBehaviour = dynamic_cast<ValueSettingsBehaviour*>(behaviour);
            if (!valueSettingsBehaviour) continue;

            if (valueSettingsBehaviour->bypassesInput(mainHandItem)) 
                continue;

            if (!valueSettingsBehaviour->mayInteract(player))
                continue;

            if (ray.mType == HitResultType::NO_HIT)
                continue;

            // if (behaviour instanceof SidedFilteringBehaviour) {
			// 	behaviour = ((SidedFilteringBehaviour) behaviour).get(ray.getDirection());
			// 	if (behaviour == null)
			// 		continue;
			// }

            if (!valueSettingsBehaviour->isActive())
                continue;

            // if (valueSettingsBehaviour.onlyVisibleWithWrench()
			// 	&& !player.getItemInHand(hand).is(Items.TOOLS_WRENCH))
			// 	continue;

            // if (valueSettingsBehaviour->getSlotPositioning().get() )
            SidedValueBoxTransform* sidedSlot = dynamic_cast<SidedValueBoxTransform*>(valueSettingsBehaviour->getSlotPositioning().get());
            if (sidedSlot) {
                if (!sidedSlot->isSideActive(block, ray.mFacing)) 
                    continue;

                sidedSlot->fromSide(ray.mFacing);
            }

            bool fakePlayer = false; // player instanceof FakePlayer;
            if (!valueSettingsBehaviour->testHit(ray.mPos) && !fakePlayer) // exited here, might be fine tho
                continue;

            ev.Cancel();

            if (!valueSettingsBehaviour->acceptsValueSettings() || fakePlayer) {
                valueSettingsBehaviour->onShortInteract(player, ray.mFacing, ray);
                return;
            }

            // if (event.getSide() == LogicalSide.CLIENT) {
			// 	BehaviourType<?> type = behaviour.getType();
			// 	CatnipServices.PLATFORM.executeOnClientOnly(() -> () -> CreateClient.VALUE_SETTINGS_HANDLER
			// 		.startInteractionWith(pos, type, hand, ray.getDirection()));
			// }

            if (isClientSide) {
                const BehaviourType& type = behaviour->getType();
                Log::Info("Run CatnipServices.PLATFORM.executeOnClientOnly(() -> () -> CreateClient.VALUE_SETTINGS_HANDLER.startInteractionWith(pos, type, hand, ray.getDirection()));");
                Log::Info("Behaviour Type: {}", type.getName());
            }
        }
    }

    static bool canInteract(const Player& player) {
        // todo check if holding shift
        // return player != null && !player.isSpectator() && !player.isShiftKeyDown() && !AdventureUtil.isAdventure(player);
        return true;
    }

    static void RegisterEvents() {
        auto& bus = Amethyst::GetEventBus();
        bus.AddListener<BeforeBlockUseEvent>(&ValueSettingsInputHandler::beforeBlockUse);
    }
};