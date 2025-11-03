#include "flywheel/api/model/Models.hpp"
#include <mc/src-client/common/client/renderer/Tessellator.hpp>

RendererReloadCache<std::shared_ptr<PartialModel>, std::shared_ptr<Model>> Models::PARTIAL = RendererReloadCache<std::shared_ptr<PartialModel>, std::shared_ptr<Model>>(
    [](Tessellator& tess, const std::shared_ptr<PartialModel>& partial) -> std::shared_ptr<Model> {
        auto model = std::make_shared<Model>();
        Model::ConfiguredMesh configuredMesh;

        configuredMesh.material = flywheel::Material();

        if (!partial->isLoaded()) {
            Log::Error("Error loading mesh from partial model, partial model {} is not loaded!", partial->identifier());
            return model;
        }

        auto geometry = partial->getGeometryInfo();

        tess.begin(mce::PrimitiveMode::QuadList, 0);

		for (auto& node : geometry->mPtr->mNodes) {
			for (auto& box : node.second.mBoxes) {
				box.Tessellate(tess, geometry->mPtr->mTextureDimensions.x, geometry->mPtr->mTextureDimensions.y);
			}
		}

		configuredMesh.mesh = std::move(tess.end(0, partial->identifier().getString(), 0));
        model->meshes.push_back(configuredMesh);
        return model;
    }
);