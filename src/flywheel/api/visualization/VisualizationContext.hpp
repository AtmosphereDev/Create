#pragma once
class InstancerProvider;
class BlockPos;
class VisualEmbedding;

/**
 * A context object passed on visual creation.
 */
class VisualizationContext {
public:
    /**
	 * @return The {@link InstancerProvider} that the visual can use to get instancers to render models.
	 */
    virtual InstancerProvider& instancerProvider() const = 0;

    /**
	 * All models render as if this position is (0, 0, 0).
	 *
	 * <p>For a Visual to appear in the correct position in the world,
	 * it must render at its actual world position minus this renderOrigin.
	 * <br>i.e. {@code be.getBlockPos() - visualizationContext.renderOrigin()}</p>
	 *
	 * <p>This exists to prevent floating point precision issues
	 * when the camera is far away from the level's origin.</p>
	 *
	 * @return The origin of the renderer as a level position.
	 */
    virtual const BlockPos& renderOrigin() const = 0;

    /**
	 * Create a new embedding to compose visuals.
	 *
	 * @param renderOrigin The renderOrigin the embedding will appear to have.
	 * @return The embedding.
	 * @see VisualEmbedding
	 */
    virtual VisualEmbedding& createEmbedding() = 0;
};