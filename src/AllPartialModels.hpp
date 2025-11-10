#pragma once
#include "flywheel/lib/model/baked/PartialModel.hpp"

class AllPartialModels {
public:
    static std::shared_ptr<PartialModel> SHAFT;
    static std::shared_ptr<PartialModel> SHAFT_HALF;
    static std::shared_ptr<PartialModel> GEARBOX;
    static std::shared_ptr<PartialModel> CREATIVE_MOTOR;
    static std::shared_ptr<PartialModel> SHAFTLESS_LARGE_COGWHEEL;
    static std::shared_ptr<PartialModel> SHAFTLESS_COGWHEEL;
    static std::shared_ptr<PartialModel> COGWHEEL_SHAFT;

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