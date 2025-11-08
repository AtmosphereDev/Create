#pragma once

// little helper for common keys like right click, shift, ettc
// since java seems to just fetch them directtly !mc.options.keyUse.isDown())

class ClientInputs {
public:
    /**
     * true if right click is held
     */
    static bool useKeyDown;

    static short mouseX;

    static short mouseY;

    static bool shiftKeyDown;

    static void AddEventListeners();
};