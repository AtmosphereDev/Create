#pragma once
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>
#include "porting/JavaBlockEntity.hpp"
#include "foundation/blockEntity/renderer/SafeBlockEntityRenderer.hpp"
#include "AllBlocks.hpp"

#include "content/kinetics/base/ShaftRenderer.hpp"
#include "content/kinetics/gearbox/GearboxRenderer.hpp"
#include "content/kinetics/motor/CreativeMotorRenderer.hpp"
#include "content/kinetics/simpleRelays/encased/EncasedCogRenderer.hpp"
#include "content/kinetics/simpleRelays/BracketedKineticBlockEntityRenderer.hpp"

class JavaBlockEntityRenderer : public BlockActorRenderer {
public:
    std::unordered_map<HashedString, std::unique_ptr<SafeBlockEntityRenderer>> mRenderers;

    JavaBlockEntityRenderer() : BlockActorRenderer() {
        mRenderers["fx_create:shaft"] = std::make_unique<ShaftRenderer>();
        mRenderers["fx_create:gearbox"] = std::make_unique<GearboxRenderer>();
        mRenderers["fx_create:creative_motor"] = std::make_unique<CreativeMotorRenderer>();
        mRenderers["fx_create:cogwheel"] = std::make_unique<BracketedKineticBlockEntityRenderer>();
        mRenderers["fx_create:large_cogwheel"] = std::make_unique<BracketedKineticBlockEntityRenderer>();
    };

    virtual void render(BaseActorRenderContext& ctx, BlockActorRenderData& data) override {
        JavaBlockEntity& actor = static_cast<JavaBlockEntity&>(data.entity);

        auto it = mRenderers.find(actor.getBlock().mLegacyBlock->mNameInfo.mFullName);
        if (it != mRenderers.end()) {
            it->second->render(*this, ctx, data);
        }
        else {
            Log::Warning("No renderer found for JavaBlockEntity with id {}", actor.getBlock().mLegacyBlock->mNameInfo.mFullName.getString());
        }
    }
};