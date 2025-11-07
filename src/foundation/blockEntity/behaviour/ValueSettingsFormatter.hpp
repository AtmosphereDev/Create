#pragma once
#include <amethyst/Imports.hpp>
#include "foundation/blockEntity/behaviour/ValueSettingsBehaviour.hpp"

class ValueSettingsFormatter {
private:
    std::function<std::string(const ValueSettingsBehaviour::ValueSettings&)> formatter;

public:
    ValueSettingsFormatter(std::function<std::string(const ValueSettingsBehaviour::ValueSettings&)> fmt)
        : formatter(fmt) {}

    std::string format(const ValueSettingsBehaviour::ValueSettings& settings) const {
        return formatter(settings);
    }
};