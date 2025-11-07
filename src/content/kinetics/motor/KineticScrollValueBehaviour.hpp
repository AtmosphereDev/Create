#pragma once
#include <string>
#include <algorithm>
#include "foundation/blockEntity/behaviour/scrollValue/ScrollValueBehaviour.hpp"

class KineticScrollValueBehaviour : public ScrollValueBehaviour {
public:
    KineticScrollValueBehaviour(std::string label, SmartBlockEntity* be, std::shared_ptr<ValueBoxTransform> slot)
        : ScrollValueBehaviour(label, be, slot) {
            withFormatter([](int v) {
                return std::to_string(std::abs(v));
            });
        }

    virtual ValueSettingsBoard createBoard(const Player& player, const HitResult& hit) override;

    virtual void setValueSettings(const Player& player, std::shared_ptr<ValueSettings> valueSetting, bool ctrlDown) override {
        int value = std::max(1, valueSetting->value);
        if (valueSetting != getValueSettings()) {
            playFeedbackSound(*this);
        }
        setValue(valueSetting->row == 0 ? -value : value);
    }

    std::string formatSettings(const ValueSettingsBehaviour::ValueSettings& settings) {
        std::string symbol = settings.row == 0 ? "\xE2\x9F\xB2" : "\xE2\x9F\xB3"; // right turning arrow / left turning arrow
        return std::string(symbol) + std::to_string(std::abs(settings.value));
    }

    virtual std::string getClipboardKey() const override {
        return "Speed";
    }
};