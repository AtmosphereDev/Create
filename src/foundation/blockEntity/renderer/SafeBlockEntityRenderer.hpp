#pragma once
#include "porting/JavaBlockEntity.hpp"
#include <mc/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src-client/common/client/game/MinecraftGame.hpp>

class BaseActorRenderContext;
class BlockActorRenderData;
class BlockActorRenderer;

class SafeBlockEntityRenderer {
public:
    bool initializedTextures;

    bool isInvalid(const JavaBlockEntity& entity) const {
        return !entity.hasLevel();
    }

    void render(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) {
        if (!initializedTextures) {
            auto textures = Amethyst::GetClientCtx().mClientInstance->mMinecraftGame->mTextures;
            loadTextures(textures);
            initializedTextures = true;
        }

        if (isInvalid((JavaBlockEntity&)data.entity)) {
            return;
        }
        
        renderSafe(self, ctx, data);
    }

    virtual void loadTextures(std::shared_ptr<mce::TextureGroup> textures) {}

    virtual void renderSafe(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) = 0;
};