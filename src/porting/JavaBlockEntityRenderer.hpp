#pragma once
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>
#include "porting/JavaBlockEntity.hpp"
#include "foundation/blockEntity/renderer/SafeBlockEntityRenderer.hpp"
#include "AllBlocks.hpp"

#include "content/kinetics/base/ShaftRenderer.hpp"
#include "content/kinetics/simpleRelays/ShaftBlock.hpp"

class JavaBlockEntityRenderer : public BlockActorRenderer {
public:
    std::unordered_map<HashedString, std::unique_ptr<SafeBlockEntityRenderer>> mRenderers;

    JavaBlockEntityRenderer() : BlockActorRenderer() {
        mRenderers["fx_create:shaft"] = std::make_unique<ShaftRenderer>();
        mRenderers["fx_create:gearbox"] = std::make_unique<ShaftRenderer>();
    };

    virtual void render(BaseActorRenderContext& ctx, BlockActorRenderData& data) override {
        JavaBlockEntity& actor = static_cast<JavaBlockEntity&>(data.entity);

        Log::Info("JavaBlockEntityRenderer rendering entity with id {}", actor.getBlock().mLegacyBlock->mNameInfo.mFullName.getString());

        auto it = mRenderers.find(actor.getBlock().mLegacyBlock->mNameInfo.mFullName);
        if (it != mRenderers.end()) {
            it->second->render(*this, ctx, data);
        }
        else {
            Log::Warning("No renderer found for JavaBlockEntity with id {}", actor.getBlock().mLegacyBlock->mNameInfo.mFullName.getString());
        }
    }
};