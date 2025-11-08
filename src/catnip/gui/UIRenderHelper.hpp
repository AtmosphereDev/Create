#pragma once
#include <amethyst/Imports.hpp>
#include <mc/src-client/common/client/renderer/TexturePtr.hpp>
#include <mc/src-client/common/client/renderer/screen/MinecraftUIRenderContext.hpp>
#include <mc/src-deps/core/resource/ResourceHelper.hpp>
#include "foundation/gui/AllGuiTextures.hpp"

class UIRenderHelper {
public:
    static std::unordered_map<HashedString, mce::TexturePtr> textureCache;

    static const mce::TexturePtr& getTexture(MinecraftUIRenderContext& ctx, const TextureSheetTexture& texture) {
        auto it = textureCache.find(texture.name);
        if (it != textureCache.end()) {
            return it->second;
        }

        mce::TexturePtr ptr = ctx.getTexture(texture.name, false);
        textureCache[texture.name] = ptr;
        return textureCache[texture.name];
    }

    static void render(MinecraftUIRenderContext& ctx, int startX, int startY, const TextureSheetTexture& texture) {
        const mce::TexturePtr& ptr = getTexture(ctx, texture);

        glm::tvec2<float> position(startX, startY);
        glm::tvec2<float> size(texture.width(), texture.height());

        ctx.drawImage(ptr, position, size, texture.uv, texture.uvSize, 0);
    }

    static void drawStretched(MinecraftUIRenderContext& ctx, int startX, int startY, int width, int height, const TextureSheetTexture& texture) {
        const mce::TexturePtr& ptr = getTexture(ctx, texture);

        glm::tvec2<float> position(startX, startY);
        glm::tvec2<float> size(width, height);

        ctx.drawImage(ptr, position, size, texture.uv, texture.uvSize, 0);
    }

    static void drawCropped(MinecraftUIRenderContext& ctx, int startX, int startY, int drawWidth, int drawHeight, const TextureSheetTexture& texture) {
        const mce::TexturePtr& ptr = getTexture(ctx, texture);

        int texWidth  = texture.width();
        int texHeight = texture.height();

        int visibleWidth  = std::min(drawWidth,  texWidth);
        int visibleHeight = std::min(drawHeight, texHeight);

        if (visibleWidth <= 0 || visibleHeight <= 0)
            return;

        int cropRight  = texWidth  - visibleWidth;
        int cropBottom = texHeight - visibleHeight;

        glm::tvec2<float> uvOffset = texture.uv; 
        glm::tvec2<float> uvSize(
            (float)visibleWidth  / texture.textureWidth,
            (float)visibleHeight / texture.textureHeight
        );

        glm::tvec2<float> position(startX, startY);
        glm::tvec2<float> size(visibleWidth, visibleHeight);

        ctx.drawImage(ptr, position, size, uvOffset, uvSize, 0);
    }
};