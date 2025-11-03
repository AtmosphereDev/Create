#pragma once
#include "flywheel/api/model/Model.hpp"
#include "flywheel/lib/model/baked/PartialModel.hpp"
#include "flywheel/lib/util/RendererReloadCache.hpp"

class Block;

class Models {
private:
    static RendererReloadCache<std::shared_ptr<PartialModel>, std::shared_ptr<Model>> PARTIAL;

    static RendererReloadCache<const Block*, std::shared_ptr<Model>> BLOCK_STATE;

public:
    static std::shared_ptr<Model> partial(Tessellator& tess, std::shared_ptr<PartialModel> partial) {
        return PARTIAL.get(tess, partial);
    }

    static std::shared_ptr<Model> block(Tessellator& tess, const Block& blockState) {
        return BLOCK_STATE.get(tess, &blockState);
    }
};