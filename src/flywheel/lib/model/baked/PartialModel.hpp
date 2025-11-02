#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>
#include <mc/src-deps/core/resource/ResourceHelper.hpp>
#include <mc/src-deps/minecraftrenderer/renderer/Mesh.hpp>

class PartialModel {
public:
    static inline bool populateOnInit = false;

    static std::shared_ptr<PartialModel> of(const ResourceLocation& modelLocation) {
        std::scoped_lock lock(allMutex_);
        auto key = modelLocation; // assumes ResourceLocation is hashable & comparable

        if (auto existingWeak = all_[key].lock()) {
            return existingWeak;
        }

        auto instance = std::shared_ptr<PartialModel>(new PartialModel(modelLocation));
        all_[key] = instance;
        return instance;
    }

    std::shared_ptr<mce::Mesh> get() const { return bakedModel_; }
    const ResourceLocation& modelLocation() const { return modelLocation_; }

private:
    explicit PartialModel(const ResourceLocation& modelLocation)
        : modelLocation_(modelLocation) {
        if (populateOnInit) {
            // bakedModel_ = FlwLibXplat::getInstance()
            //     .getBakedModel(Minecraft::getInstance().getModelManager(), modelLocation_);
        }
    }

    ResourceLocation modelLocation_;
    std::shared_ptr<mce::Mesh> bakedModel_;

    // Static global weak cache
    static inline std::unordered_map<ResourceLocation, std::weak_ptr<PartialModel>> all_;
    static inline std::mutex allMutex_;
};