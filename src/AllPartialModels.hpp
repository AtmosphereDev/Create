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
    static std::shared_ptr<PartialModel> COGWHEEL;

    static std::shared_ptr<PartialModel> BELT_PULLEY;
    static std::shared_ptr<PartialModel> BELT_START;
    static std::shared_ptr<PartialModel> BELT_MIDDLE;
    static std::shared_ptr<PartialModel> BELT_END;
    static std::shared_ptr<PartialModel> BELT_START_BOTTOM;
    static std::shared_ptr<PartialModel> BELT_MIDDLE_BOTTOM;
    static std::shared_ptr<PartialModel> BELT_END_BOTTOM;
    static std::shared_ptr<PartialModel> BELT_DIAGONAL_START;
    static std::shared_ptr<PartialModel> BELT_DIAGONAL_MIDDLE;
    static std::shared_ptr<PartialModel> BELT_DIAGONAL_END;

private:
    static std::shared_ptr<PartialModel> block(const std::string& path, const std::string& texture) {
        auto model = PartialModel::of(path, texture);
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