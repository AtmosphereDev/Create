#pragma once
#include "flywheel/lib/model/baked/PartialModel.hpp"

class AllPartialModels {
public:
    static std::shared_ptr<PartialModel> SHAFT;

private:
    static std::shared_ptr<PartialModel> block(const std::string& path) {
        return PartialModel::of(path);
    }

public:
    static void AddEventListeners();
}; 