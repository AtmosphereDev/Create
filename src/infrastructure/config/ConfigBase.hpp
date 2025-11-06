#pragma once
#include <string>
#include <algorithm>

// seems to be in some other place, just putting it here for now

template<typename T>
class ConfigValueBase {
public:
    ConfigValueBase(T defaultVal, T min, T max, std::string name)
        : val(defaultVal), min(min), max(max), name(name) {}

    void set(T v) {
        val = std::clamp(v, min, max);
    }

    T get() const {
        return val;
    }

private:
    T val;
    T min;
    T max;
    std::string name;
};

class ConfigInt : public ConfigValueBase<int> {
public:
    ConfigInt(int defaultVal, int min, std::string name)
        : ConfigValueBase(defaultVal, min, std::numeric_limits<int>::max(), name) {}
};

class ConfigFloat : public ConfigValueBase<float> {
public:
    ConfigFloat(float defaultVal, float min, float max, std::string name)
        : ConfigValueBase(defaultVal, min, max, name) {}
};