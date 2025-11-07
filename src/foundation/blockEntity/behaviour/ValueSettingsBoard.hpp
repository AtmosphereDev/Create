#pragma once
#include <amethyst/Imports.hpp>
#include "foundation/blockEntity/behaviour/ValueSettingsFormatter.hpp"

class ValueSettingsFormatter;

class ValueSettingsBoard {
public:
    std::string title;
    int maxValue;
    int milestoneInterval;
    std::vector<std::string> rows;
    ValueSettingsFormatter formatter;

    ValueSettingsBoard(const std::string& titleIn, int maxValueIn, int milestoneIntervalIn, std::vector<std::string> rows, ValueSettingsFormatter formatter)
        : title(titleIn), maxValue(maxValueIn), milestoneInterval(milestoneIntervalIn), rows(std::move(rows)), formatter(std::move(formatter)) {}
};