#include "ValueSettingsClient.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsInputHandler.hpp"
#include <mc/src-client/common/client/player/LocalPlayer.hpp>
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsBehaviour.hpp"
#include <mc/src-client/common/client/gui/screens/controllers/ClientInstanceScreenController.hpp>
#include <mc/src-client/common/client/gui/screens/SceneCreationUtils.hpp>
#include <mc/src-client/common/client/gui/screens/UIScene.hpp>
#include "ValueSettingsScreen.hpp"

void ValueSettingsClient::startInteractionWith(const BlockPos &pos, const BehaviourType &type, FacingID direction)
 {
    interactHeldTicks = 0;
    interactHeldPos = pos;
    interactHeldBehaviour = &type;
    interactHeldFace = direction;
    Log::Info("Started ValueSettings interaction at pos {} with {}", pos, type.getName());
}

void ValueSettingsClient::tick()
{
    if (mc == nullptr) {
        mc = Amethyst::GetClientCtx().mClientInstance;
        return;
    }

    if (hoverWarmup > 0)
        hoverWarmup--;
    if (hoverTicks > 0) 
        hoverTicks--;
    if (interactHeldTicks == -1)
        return;

    LocalPlayer* _player = mc->getLocalPlayer();
    if (_player == nullptr) return;

	LocalPlayer& player = *_player;

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

    Log::Info("Opening Value Settings Screen for behaviour: {}", behaviour->getType().getName());
    // ScreenOpener.open(new ValueSettingsScreen(interactHeldPos,
	// 		valueSettingBehaviour.createBoard(player, blockHitResult), valueSettingBehaviour.getValueSettings(),
	// 		valueSettingBehaviour::newSettingHovered, valueSettingBehaviour.netId()));

    SceneFactory& factory = *mc->mSceneFactory;

    auto model = SceneCreationUtils::_createModel<ClientInstanceScreenModel>(
        factory,
        *mc->mMinecraftGame,
        *mc,
        factory.mAdvancedGraphicOptions
    );

    auto controller = std::make_shared<ValueSettingsScreen>(
        model, 
        interactHeldPos.value(), 
        valueSettingsBehaviour->createBoard(player, ray),
        valueSettingsBehaviour->getValueSettings(),
        [valueSettingsBehaviour](ValueSettingsBehaviour::ValueSettings& setting) {
            valueSettingsBehaviour->newSettingHovered(setting);
        },
        valueSettingsBehaviour->netId()
    );
    
    auto scene = factory.createUIScene(*mc->mMinecraftGame, *mc, "value_settings_screen.value_settings_screen", controller);
    auto screen = factory._createScreen(scene);
    factory.getCurrentSceneStack()->pushScreen(screen, false);
	Log::Info("Value Settings Screen opened.");

    interactHeldTicks = -1;
}