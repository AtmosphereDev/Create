#pragma once
#include <functional>
#include <cmath>
#include <algorithm>
#include "catnip/math/AngleHelper.hpp"

class LerpedFloat {
public:
    using Interpolator = std::function<float(float, float, float)>;
    using Chaser = std::function<float(float, float, float)>;

private:
    Interpolator interpolator;
    float previousValue = 0.0f;
    float value = 0.0f;

    Chaser chaseFunction = nullptr;
    float chaseTarget = 0.0f;
    float chaseSpeed = 0.0f;
    bool angularChase = false;

    bool forcedSync = true;

public:
    explicit LerpedFloat(Interpolator interp)
        : interpolator(interp) {
        startWithValue(0);
    }

    static LerpedFloat linear() {
        return LerpedFloat([](float p, float c, float t) { return c + (t - c) * p; });
    }

    float getValue(float partialTicks = 1.0f) const {
        return interpolator(partialTicks, previousValue, value);
    }

    LerpedFloat& startWithValue(float v) {
        previousValue = value = chaseTarget = v;
        forcedSync = true;
        return *this;
    }

    LerpedFloat& chase(float target, float speed, Chaser chaser) {
        updateChaseTarget(target);
        chaseSpeed = speed;
        chaseFunction = chaser;
        return *this;
    }

    void updateChaseTarget(float target) {
        if (angularChase) {
            // Implement angle shortest diff if needed
            target = value + AngleHelper::getShortestAngleDiff(value, target);
        }
        chaseTarget = target;
    }

    void tickChaser() {
        previousValue = value;
        if (!chaseFunction) return;
        if (std::fabs(value - chaseTarget) < 1e-5f) {
            value = chaseTarget;
            return;
        }
        value = chaseFunction(value, chaseSpeed, chaseTarget);
    }

    void setValue(float v) { previousValue = value; value = v; }
    void setValueNoUpdate(float v) { value = v; }

    bool settled() const {
        return previousValue == value && (!chaseFunction || value == chaseTarget);
    }

    float getChaseTarget() const { return chaseTarget; }

    void forceNextSync() { forcedSync = true; }

    static Chaser ChaserIdle() { return [](float c, float, float) { return c; }; }
    static Chaser ChaserLinear() {
        return [](float c, float s, float t) { 
            float diff = t - c;
            return c + std::clamp(diff, -s, s);
        };
    }

    static Chaser ChaserExp(float maxEffectiveSpeed = std::numeric_limits<float>::max()) {
        return [maxEffectiveSpeed](float c, float s, float t) {
            float diff = (t - c) * s;
            return c + std::clamp(diff, -maxEffectiveSpeed, maxEffectiveSpeed);
        };
    }
};