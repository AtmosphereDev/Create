#include "AllPartialModels.hpp"
#include <amethyst/runtime/ModContext.hpp>

std::shared_ptr<PartialModel> AllPartialModels::SHAFT = AllPartialModels::block("geometry.fx_create.shaft");

// Model loading

#include <mc/src-client/common/client/renderer/blockActor/BlockActorRendererDispatcher.hpp>
#include <mc/src-client/common/client/model/GeometryGroup.hpp>
#include <mc/src-client/common/client/model/Geometry.hpp>
#include "content/kinetics/base/RotatedPillarKineticBlock.hpp"

SafetyHookInline _BlockActorRenderDispatcher_initializeBlockEntityRenderers;

void BlockActorRenderDispatcher_initializeBlockEntityRenderers(
	BlockActorRenderDispatcher* self, 
	const gsl::not_null<Bedrock::NonOwnerPointer<GeometryGroup>>& geometryGroup,
	std::shared_ptr<mce::TextureGroup> textures, 
	void* a4, void* a5,
	void* a6, void* a7, void* a8, void* a9
) {
    self->mRenderers[(BlockActorRendererId)((int)BlockActorRendererId::Count + 1)] = std::make_unique<TestBlockActorRenderer>();

	_BlockActorRenderDispatcher_initializeBlockEntityRenderers.call<void>(self, geometryGroup, textures, a4, a5, a6, a7, a8, a9);
}

void AllPartialModels::AddEventListeners()
{
    auto& hooks = Amethyst::GetHookManager();
    HOOK(BlockActorRenderDispatcher, initializeBlockEntityRenderers);
}
