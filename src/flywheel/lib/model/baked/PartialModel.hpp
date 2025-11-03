#pragma once
#include <memory>
#include <mutex>
#include <unordered_map>
#include <mc/src-deps/core/resource/ResourceHelper.hpp>
#include <mc/src-deps/minecraftrenderer/renderer/Mesh.hpp>
#include <mc/src-client/common/client/model/Geometry.hpp>
#include <mc/src-client/common/client/model/GeometryGroup.hpp>

class PartialModel {
public:
    static std::shared_ptr<PartialModel> of(const HashedString& identifier) {
        std::scoped_lock lock(allMutex_);
        auto key = identifier;

        if (auto existingWeak = all_[key].lock()) {
            return existingWeak;
        }

		auto instance = std::shared_ptr<PartialModel>(new PartialModel(identifier));
        all_[key] = instance;
        return instance;
    }

    const HashedString& identifier() const {
        return identifier_;
    }

    bool isLoaded() const {
        return geometryInfo_ != nullptr;
    }

    std::shared_ptr<GeometryInfo> getGeometryInfo() const {
        return geometryInfo_;
    }

    void setGeometryInfo(std::shared_ptr<GeometryInfo> geometryInfo) {
        geometryInfo_ = geometryInfo;
    }

private:
    explicit PartialModel(const HashedString& identifier)
        : identifier_(identifier), geometryInfo_(nullptr) {}

    HashedString identifier_;
    std::shared_ptr<GeometryInfo> geometryInfo_;

    // Static global weak cache
    static inline std::unordered_map<HashedString, std::weak_ptr<PartialModel>> all_;
    static inline std::mutex allMutex_;
};