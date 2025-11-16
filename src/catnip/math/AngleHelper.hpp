#pragma once
#include <mc/src/common/world/Facing.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

class AngleHelper {
public:
    // static float horizontalAngle(FacingID facing) {
    //     if (Facing::getAxis(facing) == Facing::Axis::Y) return 0.f;
    //     float angle = Facing::toYRot(facing);
    //     if (Facing::getAxis(facing) == Facing::Axis::X) angle = -angle;
    //     return angle;
    // }

    static float verticalAngle(FacingID facing) {
        return facing == FacingID::UP ? -90.f : (facing == FacingID::DOWN ? 90.f : 0.f);
    }

    static float rad(double angle) {
        if (angle == 0) return 0.f;
        return static_cast<float>(angle / 180.0 * glm::pi<double>());
    }

    static float deg(double angle) {
        if (angle == 0) return 0.f;
        return static_cast<float>(angle * 180.0 / glm::pi<double>());
    }

    static float getShortestAngleDiff(double current, double target) {
        current = fmod(current, 360.0);
        target = fmod(target, 360.0);
        return static_cast<float>(fmod((target - current + 540.0), 360.0) - 180.0);
    }

    static float getShortestAngleDiff(double current, double target, float hint) {
        float diff = getShortestAngleDiff(current, target);
        if (std::abs(diff) == 180.f && (diff * hint < 0)) {
            return diff + 360.f * (hint > 0 ? 1.f : -1.f);
        }
        return diff;
    }

    static float wrapAngle180(float angle) {
        return fmod(angle + 180.f, 360.f) - 180.f;
    }

    static float angleLerp(double pct, double current, double target) {
        return static_cast<float>(current + getShortestAngleDiff(current, target) * pct);
    }
};