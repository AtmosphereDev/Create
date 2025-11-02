#pragma once
#include <concepts>
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include "flywheel/lib/visual/AbstractBlockEntityVisual.hpp"

template <typename T> 
requires std::is_base_of_v<KineticBlockEntity, T>
class KineticBlockEntityVisual : public AbstractBlockEntityVisual<T> {
public:
    KineticBlockEntityVisual(const VisualizationContext& ctx, T* blockEntity, float partialTick) 
        : AbstractBlockEntityVisual<T>(ctx, blockEntity, partialTick) {}

    static float rotationOffset(const Block& state, Facing::Axis axis, const BlockPos& pos) {
        if (shouldOffset(axis, pos)) {
            return 22.5f;
        }
        else {
            return 0.0f;
            // return ICogWheel.isLargeCog(state) ? 11.25f : 0; TODO
        }
    }

public:
    static bool shouldOffset(Facing::Axis axis, const BlockPos& pos) {
        int x = (axis == Facing::Axis::X) ? 0.0f : pos.x;
        int y = (axis == Facing::Axis::Y) ? 0.0f : pos.y;
        int z = (axis == Facing::Axis::Z) ? 0.0f : pos.z;
        return ((x + y + z) % 2) == 0;
    }

    static Facing::Axis rotationAxis(const Block& state) {
        if (!IRotate::isIRotate(state)) return Facing::Axis::Y;
        auto& irotate = (const IRotate&)(state.mLegacyBlock);
        return irotate.getRotationAxis(state);
    }
};