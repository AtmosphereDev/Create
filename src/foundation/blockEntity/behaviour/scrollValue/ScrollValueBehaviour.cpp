#include "ScrollValueBehaviour.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsBoard.hpp"

const BehaviourType ScrollValueBehaviour::TYPE = BehaviourType("ScrollValueBehaviour");

ValueSettingsBoard ScrollValueBehaviour::createBoard(const Player &player, const HitResult &hit)
{
    std::vector<std::string> rows;
    rows.push_back("Value"); // Component.literal("Value")

    return ValueSettingsBoard(label, max, 10, rows, ValueSettingsFormatter([](const ValueSettingsBehaviour::ValueSettings& v) {
        return v.format();
    }));
}