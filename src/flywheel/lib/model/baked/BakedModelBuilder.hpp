#pragma once
#include <memory>
#include <mc/src-deps/minecraftrenderer/renderer/Mesh.hpp>

class BakedModelBuilder {
public:
    std::shared_ptr<mce::Mesh> bakedModel;
    // std::optional<BlockAndTintGetter> level;
    // std::optional<BlockPos> pos;
    // std::optional<PoseStack> pose;
    // std::optional<BiFunction<RenderType, Boolean, Material>> materialFunc;
};