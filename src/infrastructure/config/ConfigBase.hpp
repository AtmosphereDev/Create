#pragma once
#include <string>

// seems to be in some other place, just putting it here for now

class ConfigInt {
public:
    ConfigInt(int defaultVal, int min, std::string name) {};
    void set(int v);
    int get() const;
};