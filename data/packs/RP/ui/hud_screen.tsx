import { createFile, UiFile, createMinecraftElement, Panel, GetRef, Variable, Size2D, ImageProps, Image, Custom } from "Regolith-Generators"

const hud_screen = new UiFile("hud");

hud_screen.addControl("value_settings_screen_tip_renderer",
    <Custom
        renderer="value_settings_screen_tip_renderer"
        size={["100%", "100%"]}
    />
)

hud_screen.addControl("root_panel",
    <Panel modifications={[
        {
            operation: "insert_front",
            array_name: "controls",
            value: [
                { "value_settings_screen_tip_renderer@hud.value_settings_screen_tip_renderer": {} }
            ]
        }
    ]} />
)

createFile(hud_screen.toJson());