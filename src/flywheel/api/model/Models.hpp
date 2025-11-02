#pragma once
#include "flywheel/api/model/Model.hpp"
#include "flywheel/lib/model/baked/PartialModel.hpp"
#include "flywheel/lib/util/RendererReloadCache.hpp"

class Models {
public:
    static RendererReloadCache<std::shared_ptr<PartialModel>, std::shared_ptr<Model>> PARTIAL;

    static std::shared_ptr<Model> partial(std::shared_ptr<PartialModel> partial) {
        return PARTIAL.get(partial);
    }
};