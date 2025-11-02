#include "flywheel/api/model/Models.hpp"

RendererReloadCache<std::shared_ptr<PartialModel>, std::shared_ptr<Model>> Models::PARTIAL = RendererReloadCache<std::shared_ptr<PartialModel>, std::shared_ptr<Model>>(
    [](const std::shared_ptr<PartialModel>& partial) -> std::shared_ptr<Model> {
        auto model = std::make_shared<Model>();
        Model::ConfiguredMesh configuredMesh;

        configuredMesh.material = flywheel::Material();

        if (auto mesh = partial->get()) {
            configuredMesh.mesh = *mesh;
        }

        model->meshes.push_back(std::move(configuredMesh));
        return model;
    }
);