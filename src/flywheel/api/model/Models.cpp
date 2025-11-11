#include "flywheel/api/model/Models.hpp"
#include <mc/src-client/common/client/renderer/Tessellator.hpp>
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src-client/common/client/game/MinecraftGame.hpp>

RendererReloadCache<std::shared_ptr<PartialModel>, std::shared_ptr<Model>> Models::PARTIAL = RendererReloadCache<std::shared_ptr<PartialModel>, std::shared_ptr<Model>>(
    [](Tessellator& tess, const std::shared_ptr<PartialModel>& partial) -> std::shared_ptr<Model> {
        auto model = std::make_shared<Model>();
        Model::ConfiguredMesh configuredMesh;

        // configuredMesh.material = flywheel::Material();

        if (!partial->isLoaded()) {
            Log::Error("Error loading mesh from partial model, partial model {} is not loaded!", partial->identifier());
            return model;
        }

        auto geometry = partial->getGeometryInfo();

        tess.begin(mce::PrimitiveMode::QuadList, 0);

        geometry->mPtr->Tessellate(tess);
		configuredMesh.mesh = std::move(tess.end(0, partial->identifier().getString(), 0));

        auto textures = Amethyst::GetClientCtx().mClientInstance->mMinecraftGame->mTextures;
        configuredMesh.texture = textures->getTexture(partial->getTexture(), false, std::nullopt, cg::TextureSetLayerType::Normal);

        model->meshes.push_back(configuredMesh);
        return model;
    }
);