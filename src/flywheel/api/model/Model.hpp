#pragma once
#include "flywheel/api/material/Material.hpp"
#include <mc/src-deps/minecraftrenderer/renderer/Mesh.hpp>
#include <mc/src-client/common/client/renderer/BaseActorRenderContext.hpp>
#include <mc/src-client/common/client/renderer/screen/ScreenContext.hpp>

class Model {
public:
    struct ConfiguredMesh {
        mce::Mesh mesh;
		mce::TexturePtr texture;
    };

    /**
	 * Get a list of all meshes in this model.
	 *
	 * <p>The contents of the returned list will be queried, but never modified.</p>
	 *
	 * <p>Meshes will be rendered in the order they appear in this list. See
	 * {@link InstancerProvider#instancer(InstanceType, Model, int)} for a complete explanation</p>
	 *
	 * @return A list of meshes.
	 */
    std::vector<ConfiguredMesh> meshes;

    /**
	 * Get a vec4 representing this model's bounding sphere in the format (x, y, z, radius).
	 * It should encompass all meshes' bounding spheres.
	 *
	 * @return A vec4 view.
	 */
	// Vector4fc boundingSphere();

	void render(BaseActorRenderContext& ctx, const mce::MaterialPtr& material) {
		for (const auto& mesh : meshes) {
			mesh.mesh.renderMesh(ctx.mScreenContext, material, mesh.texture);
		}
	}
};