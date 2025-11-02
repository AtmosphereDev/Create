#pragma once
#include <cstdint>
#include "flywheel/lib/instance/ColoredLitOverlayInstance.hpp"
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include "content/kinetics/base/KineticBlockEntityVisual.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class RotatingInstance : public ColoredLitOverlayInstance {
public:
    static constexpr float SPEED_MULTIPLIER = 6.0f;

    int8_t rotationAxisX;
    int8_t rotationAxisY;
    int8_t rotationAxisZ;
    float x;
    float y;
    float z;

    /**
	 * Speed in degrees per second
	 */
	float rotationalSpeed;

    /**
	 * Offset in degrees
	 */
	float rotationOffset;

    /**
	 * Base rotation of the instance, applied before kinetic rotation
	 */
	glm::quat rotation = glm::quat();

    RotatingInstance& setup(KineticBlockEntity* blockEntity) {
        const Block& blockState = blockEntity->getBlock();
        Facing::Axis axis = KineticBlockEntityVisual<KineticBlockEntity>::rotationAxis(blockState);
        return setup(blockEntity, axis, blockEntity->getSpeed());
    }

    RotatingInstance& setup(KineticBlockEntity* blockEntity, Facing::Axis axis) {
        return setup(blockEntity, axis, blockEntity->getSpeed());
    }

    RotatingInstance& setup(KineticBlockEntity* blockEntity, float speed) {
        const Block& blockState = blockEntity->getBlock();
        Facing::Axis axis = KineticBlockEntityVisual<KineticBlockEntity>::rotationAxis(blockState);
        return setup(blockEntity, axis, speed);
    }

    RotatingInstance& setup(KineticBlockEntity* blockEntity, Facing::Axis axis, float speed) {
        const Block& blockState = blockEntity->getBlock();
        const BlockPos& pos = blockEntity->getBlockPos();

        setRotationAxis(axis)
            .setRotationalSpeed(speed * RotatingInstance::SPEED_MULTIPLIER)
            .setRotationOffset(
                KineticBlockEntityVisual<KineticBlockEntity>::rotationOffset(blockState, axis, pos) + blockEntity->getRotationAngleOffset(axis)
            );

        // if (KineticDebugger.isActive())
		// 	instance.setColor(blockEntity);

        return *this;
    }

    RotatingInstance& rotateToFace(Facing::Axis axis) {
        FacingID orientation = Facing::fromDirectionAndAxis(Facing::AxisDirection::POSITIVE, axis);
        return rotateToFace(orientation);
    }

    RotatingInstance& rotateToFace(FacingID face) {
        BlockPos offset = Facing::getOffset(face);
        return rotateToFace(offset.x, offset.y, offset.z);
    }

    RotatingInstance& rotateToFace(float stepX, float stepY, float stepZ) {
        return rotateTo(0.0f, 1.0f, 0.0f, stepX, stepY, stepZ);
    }

    RotatingInstance& rotateTo(float fromX, float fromY, float fromZ, float toX, float toY, float toZ) {
        glm::vec3 from = glm::normalize(glm::vec3(fromX, fromY, fromZ));
        glm::vec3 to   = glm::normalize(glm::vec3(toX, toY, toZ));

        if (glm::length(from - to) < 1e-6f) {
            rotation = glm::quat(1, 0, 0, 0); // identity
        } else {
            rotation = glm::rotation(from, to);
        }
        return *this;
    }

    RotatingInstance& setRotationAxis(Facing::Axis axis) {
        return setRotationAxis(Facing::getOffset(
            Facing::fromDirectionAndAxis(Facing::AxisDirection::POSITIVE, axis))
        );
    }

    RotatingInstance& setRotationAxis(const Vec3& axis) {
        this->rotationAxisX = static_cast<int8_t>(axis.x * 127);
        this->rotationAxisY = static_cast<int8_t>(axis.y * 127);
        this->rotationAxisZ = static_cast<int8_t>(axis.z * 127);
        return *this;
    }

    RotatingInstance& setRotationalSpeed(float speed) {
        this->rotationalSpeed = speed;
        return *this;
    }

    RotatingInstance& setRotationOffset(float offset) {
        this->rotationOffset = offset;
        return *this;
    }
};