import { Custom, GetRef, Label, Panel, PanelProps, ScreenProps, UiFile, createFile, createMinecraftElement } from "Regolith-Generators"

const screen = new UiFile("value_settings_screen");

const BaseScreen = GetRef<ScreenProps & { $screen_animations: unknown }>("common", "base_screen");
const RootPanel = GetRef<PanelProps>("common", "root_panel");

screen.addControl("main_content",
    <RootPanel>
        <Panel size={["100%", "100%"]}>
            <Custom renderer="value_settings_screen_renderer" size={["100%", "100%"]} />
        </Panel>
    </RootPanel>
)

screen.addControl("value_settings_screen",
    <BaseScreen variables={[
        {
            requires: "$desktop_screen",
            $screen_content: "value_settings_screen.main_content",
            $screen_bg_content: "common.screen_background",
            $screen_background_alpha: 0.4
        }
    ]}
    force_render_below
    render_game_behind
    $screen_animations={[
        "@common.screen_exit_animation_push_fade",
        "@common.screen_exit_animation_pop_fade",
        "@common.screen_entrance_animation_push_fade",
        "@common.screen_entrance_animation_pop_fade"
    ]}
    />
)

createFile(screen.toJson())