#pragma once
#include <concepts>
#include "porting/JavaBlockEntity.hpp"
#include "flywheel/lib/visual/AbstractVisual.hpp"

/**
 * The layer between a {@link BlockEntity} and the Flywheel backend.
 * <br>
 * <br> There are a few additional features that overriding classes can opt in to:
 * <ul>
 *     <li>{@link DynamicVisual}</li>
 *     <li>{@link TickableVisual}</li>
 *     <li>{@link LightUpdatedVisual}</li>
 *     <li>{@link ShaderLightVisual}</li>
 * </ul>
 * See the interfaces' documentation for more information about each one.
 *
 * <br> Implementing one or more of these will give an {@link AbstractBlockEntityVisual} access
 * to more interesting and regular points within a tick or a frame.
 *
 * @param <T> The type of {@link BlockEntity}.
 */
template <typename T>
requires std::is_base_of_v<JavaBlockEntity, T>
class AbstractBlockEntityVisual : public AbstractVisual {
protected:
    T* blockEntity;
    BlockPos pos;
    BlockPos visualPos;
    const Block& blockState;

public:
    AbstractBlockEntityVisual(VisualizationContext& ctx, T* blockEntity, float partialTick) 
        : AbstractVisual(ctx, blockEntity->getLevel(), partialTick), 
        blockEntity(blockEntity), 
        blockState(blockEntity->getBlock()), 
        pos(blockEntity->getBlockPos()),
        visualPos(blockEntity->getBlockPos() - ctx.renderOrigin()) {}

    const BlockPos& getBlockPos() const {
        return pos;
    }

    const BlockPos& getVisualPosition() const {
        return visualPos;
    }
};