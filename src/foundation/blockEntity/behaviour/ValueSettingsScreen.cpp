#include "ValueSettingsScreen.hpp"

ValueSettingsScreen* ValueSettingsScreen::instance = nullptr;

void ValueSettingsScreen::AddEventListeners()
{
    auto& uiRendererReg = Amethyst::GetClientCtx().mCustomUIRendererRegistry;

    uiRendererReg->registerRenderer("value_settings_screen_renderer", []() {
        return std::make_shared<ValueSettingsScreenRenderer>();
    });
}