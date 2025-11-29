#include "AllPartialModels.hpp"
#include <amethyst/runtime/ModContext.hpp>

std::shared_ptr<PartialModel> AllPartialModels::SHAFT = AllPartialModels::block("geometry.fx_create.shaft", "textures/entity/axis");
std::shared_ptr<PartialModel> AllPartialModels::SHAFT_HALF = AllPartialModels::block("geometry.fx_create.shaft_half", "textures/entity/axis");
std::shared_ptr<PartialModel> AllPartialModels::GEARBOX = AllPartialModels::block("geometry.fx_create.gearbox", "textures/entity/gearbox");
std::shared_ptr<PartialModel> AllPartialModels::CREATIVE_MOTOR = AllPartialModels::block("geometry.fx_create.creative_motor", "textures/entity/creative_motor");
std::shared_ptr<PartialModel> AllPartialModels::SHAFTLESS_LARGE_COGWHEEL = AllPartialModels::block("geometry.fx_create.large_cogwheel_shaftless", "textures/entity/large_cogwheel");
std::shared_ptr<PartialModel> AllPartialModels::SHAFTLESS_COGWHEEL = AllPartialModels::block("geometry.fx_create.cogwheel_shaftless", "textures/entity/shaftless_cogwheel");
std::shared_ptr<PartialModel> AllPartialModels::COGWHEEL_SHAFT = AllPartialModels::block("geometry.fx_create.cogwheel_shaft", "textures/entity/axis");
std::shared_ptr<PartialModel> AllPartialModels::COGWHEEL = AllPartialModels::block("geometry.fx_create.cogwheel", "textures/entity/cogwheel");

std::shared_ptr<PartialModel> AllPartialModels::BELT_PULLEY = AllPartialModels::block("geometry.fx_create.belt_pulley", "textures/entity/belt_pulley");
std::shared_ptr<PartialModel> AllPartialModels::BELT_START = AllPartialModels::block("geometry.fx_create.belt_start", "textures/entity/belt");
std::shared_ptr<PartialModel> AllPartialModels::BELT_MIDDLE = AllPartialModels::block("geometry.fx_create.belt_middle", "textures/entity/belt");
std::shared_ptr<PartialModel> AllPartialModels::BELT_END = AllPartialModels::block("geometry.fx_create.belt_end", "textures/entity/belt");
std::shared_ptr<PartialModel> AllPartialModels::BELT_START_BOTTOM = AllPartialModels::block("geometry.fx_create.belt_start_bottom", "textures/entity/belt");
std::shared_ptr<PartialModel> AllPartialModels::BELT_MIDDLE_BOTTOM = AllPartialModels::block("geometry.fx_create.belt_middle_bottom", "textures/entity/belt");
std::shared_ptr<PartialModel> AllPartialModels::BELT_END_BOTTOM = AllPartialModels::block("geometry.fx_create.belt_end_bottom", "textures/entity/belt");
std::shared_ptr<PartialModel> AllPartialModels::BELT_DIAGONAL_START = AllPartialModels::block("geometry.fx_create.belt_diagonal_start", "textures/entity/belt_diagonal");
std::shared_ptr<PartialModel> AllPartialModels::BELT_DIAGONAL_MIDDLE = AllPartialModels::block("geometry.fx_create.belt_diagonal_middle", "textures/entity/belt_diagonal");
std::shared_ptr<PartialModel> AllPartialModels::BELT_DIAGONAL_END = AllPartialModels::block("geometry.fx_create.belt_diagonal_end", "textures/entity/belt_diagonal");

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
