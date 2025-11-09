#pragma once
#include <string>
#include <mc/src/common/world/level/Level.hpp>

class SoundEntry {
public:
    std::string name;
    float basePitch = 1.0f;
    float baseVolume = 1.0f;

    SoundEntry(const std::string& name) : name(name) {}

    SoundEntry(const std::string& name, float baseVolume, float basePitch)
        : name(name), baseVolume(baseVolume), basePitch(basePitch) {}

    void play(Level& level, const Vec3& position, float volume, float pitch) const {
        level.playSound(name, position, baseVolume * volume, basePitch * pitch);
    }
};

class AllSoundEvents {
public:
    inline static SoundEntry SCROLL_VALUE = SoundEntry{"note.hat"};
};