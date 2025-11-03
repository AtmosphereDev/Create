#pragma once
#include "flywheel/lib/model/baked/PartialModel.hpp"

class AllPartialModels {
public:
    static std::shared_ptr<PartialModel> SHAFT;

private:
    static std::shared_ptr<PartialModel> block(const std::string& path) {
        auto model = PartialModel::of(path);
        allModels.push_back(model);
        return model;
    }
    
    static std::vector<std::shared_ptr<PartialModel>> allModels;

public:
    static const std::vector<std::shared_ptr<PartialModel>>& getAllModels() {
        return allModels;
    }

    static void AddEventListeners();
}; 