#pragma once
#include "porting/JavaBlockEntity.hpp"
#include <mc/src-client/common/client/renderer/blockActor/BlockActorRenderer.hpp>

class BaseActorRenderContext;
class BlockActorRenderData;
class BlockActorRenderer;

class SafeBlockEntityRenderer {
public:
    bool isInvalid(const JavaBlockEntity& entity) const {
        return !entity.hasLevel();
    }

    void render(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) const {
        if (isInvalid((JavaBlockEntity&)data.entity)) {
            return;
        }
        
        renderSafe(self, ctx, data);
    }

    virtual void renderSafe(BlockActorRenderer& self, BaseActorRenderContext& ctx, BlockActorRenderData& data) const = 0;
};