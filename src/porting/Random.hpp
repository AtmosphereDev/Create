#pragma once
#include <random>

class Random {
public:
    /// 0 to max (inclusive)
    static int nextInt(int max) {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<> dist(0, max);
        return dist(gen);
    }

    /// 0.0f to max (inclusive)
    static float nextFloat(float max) {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.0f, max);
        return dist(gen);
    }
};