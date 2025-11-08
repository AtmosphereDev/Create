#include "ValueSettingsClient.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsInputHandler.hpp"
#include <mc/src-client/common/client/player/LocalPlayer.hpp>

void ValueSettingsClient::tick()
{
    if (hoverWarmup > 0)
        hoverWarmup--;
    if (hoverTicks > 0) 
        hoverTicks--;
    if (interactHeldTicks != -1)
        return;

    LocalPlayer& player = *mc->getLocalPlayer();

    if (!ValueSettingsInputHandler::canInteract(player) /* || AllBlocks.CLIPBOARD.isIn(player.getMainHandItem()) */) {
        cancelInteraction();
        return;
    }

    const HitResult& ray = player.getLevel()->getHitResult();
    if (ray.mType == HitResultType::NO_HIT || ray.mBlock != interactHeldPos) {
        cancelInteraction();
        return;
    }

    BlockEntityBehaviour* behaviour = BlockEntityBehaviour::get<BlockEntityBehaviour>(
        player.getDimensionBlockSourceConst(), interactHeldPos.value(), *interactHeldBehaviour.value()).get();

    const auto& playerInventory = player.getSupplies();
    const auto& mainHandItem = playerInventory.getSelectedItem();
    
    ValueSettingsBehaviour* valueSettingsBehaviour = dynamic_cast<ValueSettingsBehaviour*>(behaviour);
    if (!valueSettingsBehaviour || valueSettingsBehaviour->bypassesInput(mainHandItem) || !valueSettingsBehaviour->testHit(ray.mPos)) {
        cancelInteraction();
        return;
    }

    // if (!mc.options.keyUse.isDown()) {
    //     CatnipServices.NETWORK.sendToServer(new ValueSettingsPacket(interactHeldPos, 0, 0, interactHeldHand, blockHitResult,
    //             interactHeldFace, false, valueSettingBehaviour.netId()));
    //     valueSettingBehaviour.onShortInteract(player, interactHeldHand, interactHeldFace, blockHitResult);
    //     cancelInteraction();
    //     return;
    // }

    if (interactHeldTicks > 3)
        player.mSwinging = false;
    if (interactHeldTicks++ < 5)
        return;

    Log::Info("Opening Value Settings Screen for behaviour: {}", behaviour->getType());
    // ScreenOpener.open(new ValueSettingsScreen(interactHeldPos,
	// 		valueSettingBehaviour.createBoard(player, blockHitResult), valueSettingBehaviour.getValueSettings(),
	// 		valueSettingBehaviour::newSettingHovered, valueSettingBehaviour.netId()));
    interactHeldTicks = -1;
}