#include "AllPartialModels.hpp"
#include <amethyst/runtime/ModContext.hpp>

std::shared_ptr<PartialModel> AllPartialModels::SHAFT = AllPartialModels::block("geometry.fx_create.shaft");


// Model loading

std::vector<std::shared_ptr<PartialModel>> AllPartialModels::allModels;	

#include <mc/src-client/common/client/renderer/blockActor/BlockActorRendererDispatcher.hpp>
#include <mc/src-client/common/client/model/GeometryGroup.hpp>
#include <mc/src-client/common/client/model/Geometry.hpp>
#include "content/kinetics/base/RotatedPillarKineticBlock.hpp"
#include <mc/src-client/common/client/renderer/MinecraftGameplayGraphicsResources.hpp>
#include <mc/src-client/common/client/renderer/Tessellator.hpp>	
#include "porting/JavaBlockEntityRenderer.hpp"

SafetyHookInline _BlockActorRenderDispatcher_initializeBlockEntityRenderers;

void BlockActorRenderDispatcher_initializeBlockEntityRenderers(
	BlockActorRenderDispatcher* self, 
	const gsl::not_null<Bedrock::NonOwnerPointer<GeometryGroup>>& geometryGroup,
	std::shared_ptr<mce::TextureGroup> textures, 
	void* blockTes, 
	void* actorResDefGroup,
	void* resourcePackManager, 
	MinecraftGameplayGraphicsResources& mcGameplayGraphicsResources, 
	void* a8, 
	void* a9
) {
	Log::Info("AllPartialModels::initializeBlockEntityRenderers!");
    self->mRenderers[(BlockActorRendererId)((int)BlockActorRendererId::Count + 1)] = std::make_unique<JavaBlockEntityRenderer>();

	for (const auto& model : AllPartialModels::getAllModels()) {
		auto geometry = geometryGroup->getGeometry(model->identifier());
		if (!geometry) {
			Log::Error("Failed to find geometry {} for partial model", model->identifier());
			continue;
		}

		model->setGeometryInfo(geometry);
	}

	_BlockActorRenderDispatcher_initializeBlockEntityRenderers.call<void, BlockActorRenderDispatcher*, const gsl::not_null<Bedrock::NonOwnerPointer<GeometryGroup>>&, 
		std::shared_ptr<mce::TextureGroup>, void*, void*, void*, MinecraftGameplayGraphicsResources&, void*, void*>
		(self, geometryGroup, textures, blockTes, actorResDefGroup, resourcePackManager, mcGameplayGraphicsResources, a8, a9);
}

void AllPartialModels::AddEventListeners()
{
    auto& hooks = Amethyst::GetHookManager();
    HOOK(BlockActorRenderDispatcher, initializeBlockEntityRenderers);
}
