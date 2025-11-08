#pragma once
#include <amethyst/Imports.hpp>

class TextureSheetTexture {
public:
    HashedString name;
    glm::tvec2<float> uv;       // top-left UV coordinates (normalized)
    glm::tvec2<float> uvSize;   // width/height in UV space (normalized)

    int textureWidth;
    int textureHeight;

    int width() const {
        return static_cast<int>(uvSize.x * textureWidth);
    }

    int height() const {
        return static_cast<int>(uvSize.y * textureHeight);
    }

public:
    TextureSheetTexture(
        HashedString name,
        int textureWidth, int textureHeight,
        int startX, int startY,
        int width, int height
    )
        : name(name),
          textureWidth(textureWidth),
          textureHeight(textureHeight)
    {
        uv = glm::tvec2<float>(
            startX / float(textureWidth), 
            startY / float(textureHeight)
        );
        uvSize = glm::tvec2<float>((float)width / float(textureWidth), (float)height / float(textureHeight));
    }
};


class AllGuiTextures {
public:
    inline static TextureSheetTexture BRASS_FRAME_TL      {"textures/ui/value_settings", 256, 256, 65, 9, 4, 4};
    inline static TextureSheetTexture BRASS_FRAME_TR      {"textures/ui/value_settings", 256, 256, 70, 9, 4, 4};
    inline static TextureSheetTexture BRASS_FRAME_BL      {"textures/ui/value_settings", 256, 256, 65, 19, 4, 4};
    inline static TextureSheetTexture BRASS_FRAME_BR      {"textures/ui/value_settings", 256, 256, 70, 19, 4, 4};
    inline static TextureSheetTexture BRASS_FRAME_LEFT    {"textures/ui/value_settings", 256, 256, 65, 14, 3, 4};
    inline static TextureSheetTexture BRASS_FRAME_RIGHT   {"textures/ui/value_settings", 256, 256, 71, 14, 3, 4};
    inline static TextureSheetTexture BRASS_FRAME_TOP     {"textures/ui/value_settings", 256, 256, 0, 24, 256, 3};
    inline static TextureSheetTexture BRASS_FRAME_BOTTOM  {"textures/ui/value_settings", 256, 256, 0, 27, 256, 3};

    inline static TextureSheetTexture VALUE_SETTINGS_MILESTONE     {"textures/ui/value_settings", 256, 256, 0, 0, 7, 8};
    inline static TextureSheetTexture VALUE_SETTINGS_WIDE_MILESTONE{"textures/ui/value_settings", 256, 256, 75, 14, 13, 8};
    inline static TextureSheetTexture VALUE_SETTINGS_BAR           {"textures/ui/value_settings", 256, 256, 7, 0, 249, 8};
    inline static TextureSheetTexture VALUE_SETTINGS_BAR_BG        {"textures/ui/value_settings", 256, 256, 75, 9, 1, 1};
    inline static TextureSheetTexture VALUE_SETTINGS_OUTER_BG      {"textures/ui/value_settings", 256, 256, 80, 9, 1, 1};
    inline static TextureSheetTexture VALUE_SETTINGS_CURSOR_LEFT   {"textures/ui/value_settings", 256, 256, 0, 9, 3, 14};
    inline static TextureSheetTexture VALUE_SETTINGS_CURSOR        {"textures/ui/value_settings", 256, 256, 4, 9, 56, 14};
    inline static TextureSheetTexture VALUE_SETTINGS_CURSOR_RIGHT  {"textures/ui/value_settings", 256, 256, 61, 9, 3, 14};
    inline static TextureSheetTexture VALUE_SETTINGS_CURSOR_ICON   {"textures/ui/value_settings", 256, 256, 0, 44, 22, 20};
    inline static TextureSheetTexture VALUE_SETTINGS_LABEL_BG      {"textures/ui/value_settings", 256, 256, 0, 31, 161, 11};
};