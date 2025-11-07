#include "KineticScrollValueBehaviour.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsBoard.hpp"

ValueSettingsBoard KineticScrollValueBehaviour::createBoard(const Player &player, const HitResult &hit)
{
    std::vector<std::string> rows = std::vector<std::string>();
    rows.push_back("\xE2\x9F\xB2"); // right turning arrow

    ValueSettingsFormatter formatter = ValueSettingsFormatter([this](const ValueSettingsBehaviour::ValueSettings& v) {
        return this->formatSettings(v);
    });

    return ValueSettingsBoard(label, 256, 32, rows, formatter);
}