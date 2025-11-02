#pragma once

/**
 * A general interface providing information about any type of thing that could use Flywheel's visualized rendering.
 *
 * @see DynamicVisual
 * @see TickableVisual
 * @see LightUpdatedVisual
 * @see ShaderLightVisual
 */
class Visual {
public:
    /**
	 * Update instances here.
	 *
	 * <p>Good for when instances don't change very often and when animations are GPU based.
	 *
	 * <br>If your animations are complex or more CPU driven, see {@link DynamicVisual} or {@link TickableVisual}.</p>
	 */
	virtual void update(float partialTick) = 0;

	/**
	 * Free any acquired resources.
	 */
	virtual void deleteVisual() = 0;
};