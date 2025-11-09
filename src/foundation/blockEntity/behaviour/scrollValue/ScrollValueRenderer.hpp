#pragma once
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src-client/common/client/game/ClientInstance.hpp>
#include <mc/src-client/common/client/player/LocalPlayer.hpp>
#include "foundation/blockEntity/SmartBlockEntity.hpp"
#include "foundation/blockEntity/behaviour/scrollValue/ScrollValueBehaviour.hpp"
#include "CreateClient.hpp"

class ScrollValueRenderer {
public:
    static void tick() {
        ClientInstance& ci = *Amethyst::GetClientCtx().mClientInstance;
        LocalPlayer* player = ci.getLocalPlayer();
        if (!player) return;

        Level* level = player->getLevel();
        const HitResult& hit = level->getHitResult();

        if (hit.mType != HitResultType::TILE) return;

        const BlockPos& pos = hit.mBlock;
        FacingID face = hit.mFacing;
        bool highlightFound = false;

        const BlockSource& region = player->getDimensionBlockSourceConst();
        const BlockActor* blockEntity = region.getBlockEntity(pos);

        const SmartBlockEntity* sbe = SmartBlockEntity::AsSmartBlockEntity(blockEntity);
        if (!sbe) return;

        for (const auto& [type, behaviourShared] : sbe->getAllBehaviours()) {
            ScrollValueBehaviour* behaviour = dynamic_cast<ScrollValueBehaviour*>(behaviourShared.get());
            if (!behaviour) continue;

            if (!behaviour->isActive()) {
                // Outliner.getInstance().remove(behaviour)
                continue;
            }

            const ItemStack& mainhandItem = player->getSupplies().getSelectedItem();
            bool clipboard = behaviour->bypassesInput(mainhandItem);

            // if (behaviour.needsWrench && !AllItems.WRENCH.isIn(mainhandItem) && !clipboard)
			// 	continue;

            // bool highlight = behaviour->testHit(hit.mPos) && !clipboard && !highlightFound;
            bool highlight = true;

            // if (behaviour instanceof BulkScrollValueBehaviour bulkScrolling && AllKeys.ctrlDown()) {
			// 	for (SmartBlockEntity smartBlockEntity : bulkScrolling.getBulk()) {
			// 		ScrollValueBehaviour other = smartBlockEntity.getBehaviour(ScrollValueBehaviour.TYPE);
			// 		if (other != null)
			// 			addBox(world, smartBlockEntity.getBlockPos(), face, other, highlight);
			// 	}
			// } 
            // else {
            //     addBox(world, pos, face, behaviour, highlight);
            // }
            
            if (!highlight) continue;

            highlightFound = true;
            std::vector<std::string> tip;
            tip.push_back(behaviour->label);
            tip.push_back("gui.value_settings.hold_to_edit");
            CreateClient::VALUE_SETTINGS_HANDLER.showHoverTip(std::move(tip));
        }
    }
};