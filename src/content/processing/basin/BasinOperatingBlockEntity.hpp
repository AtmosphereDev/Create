#pragma once
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include "foundation/blockEntity/behaviour/simple/DeferralBehaviour.hpp"

class BasinOperatingBlockEntity : public KineticBlockEntity {
public:
    std::shared_ptr<DeferralBehaviour> basinChecker;
    bool basinRemoved;
    // currentRecipe

    BasinOperatingBlockEntity(const BlockPos& pos, const std::string& id)
        : KineticBlockEntity(pos, id), basinRemoved(false) {}

    virtual void addBehaviours(std::vector<std::shared_ptr<BlockEntityBehaviour>>& behavioursList) override {
        KineticBlockEntity::addBehaviours(behavioursList);
        basinChecker = std::make_shared<DeferralBehaviour>(this->getShared<SmartBlockEntity>(), std::bind(&BasinOperatingBlockEntity::updateBasin, this));
        behavioursList.push_back(basinChecker);
    }

    virtual void onSpeedChanged(float prevSpeed) override {
        KineticBlockEntity::onSpeedChanged(prevSpeed);
        // if (getSpeed() == 0) basinRemoved = true; // this line is in the original, but literally pointless
        basinRemoved = false;
        basinChecker->scheduleUpdate();
    }

    virtual void tick(BlockSource& region) override {
        if (basinRemoved) {
            basinRemoved = false;
            onBasinRemoved();
            sendData();
            return;
        }

        KineticBlockEntity::tick(region);
    }

protected:
    bool updateBasin() {
        return true; // temp
    }

    virtual bool isRunning() const = 0;

    virtual void startProcessingBasin() {};

    virtual bool continueWithPreviousRecipe() {
        return true;
    }

protected:
    // 	protected <I extends RecipeInput> boolean matchBasinRecipe(Recipe<I> recipe) {
	// 	if (recipe == null)
	// 		return false;
	// 	Optional<BasinBlockEntity> basin = getBasin();
	// 	if (!basin.isPresent())
	// 		return false;
	// 	return BasinRecipe.match(basin.get(), recipe);
	// }

    void applyBasinRecipe() {
        // if (currentRecipe == null)
		// 	return;

		// Optional<BasinBlockEntity> optionalBasin = getBasin();
		// if (!optionalBasin.isPresent())
		// 	return;
		// BasinBlockEntity basin = optionalBasin.get();
		// boolean wasEmpty = basin.canContinueProcessing();
		// if (!BasinRecipe.apply(basin, currentRecipe))
		// 	return;
		// getProcessedRecipeTrigger().ifPresent(this::award);
		// basin.inputTank.sendDataImmediately();

		// // Continue mixing
		// if (wasEmpty && matchBasinRecipe(currentRecipe)) {
		// 	continueWithPreviousRecipe();
		// 	sendData();
		// }

		// basin.notifyChangeOfContents();
    }

    // std::vector<Recipe> getMatchingRecipes() {
        // Optional<BasinBlockEntity> $basin = getBasin();
		// BasinBlockEntity basin;
		// if ($basin.isEmpty() || (basin = $basin.get()).isEmpty())
		// 	return new ArrayList<>();

		// List<Recipe<?>> list = new ArrayList<>();
		// try {

		// 	IItemHandler availableItems = level.getCapability(ItemHandler.BLOCK, basin.getBlockPos(), null);
		// 	IFluidHandler availableFluids = level.getCapability(FluidHandler.BLOCK, basin.getBlockPos(), null);

		// 	// no point even searching, since no recipe will ever match
		// 	if (availableItems == null && availableFluids == null) {
		// 		return list;
		// 	}

		// 	RecipeTrie<?> trie = RecipeTrieFinder.get(getRecipeCacheKey(), level, this::matchStaticFilters);
		// 	Set<AbstractVariant> availableVariants = RecipeTrie.getVariants(availableItems, availableFluids);

		// 	for (Recipe<?> r : trie.lookup(availableVariants))
		// 		if (matchBasinRecipe(r))
		// 			list.add(r);
		// } catch (Exception e) {
		// 	Create.LOGGER.error("Failed to get recipe trie, falling back to slow logic", e);
		// 	list.clear();

		// 	for (RecipeHolder<? extends Recipe<?>> r : RecipeFinder.get(getRecipeCacheKey(), level, this::matchStaticFilters))
		// 		if (matchBasinRecipe(r.value()))
		// 			list.add(r.value());
		// }

		// list.sort((r1, r2) -> r2.getIngredients().size() - r1.getIngredients().size());

		// return list;
    // }

    virtual void onBasinRemoved() = 0;

    // 	protected Optional<BasinBlockEntity> getBasin() {
	// 	if (level == null)
	// 		return Optional.empty();
	// 	BlockEntity basinBE = level.getBlockEntity(worldPosition.below(2));
	// 	if (!(basinBE instanceof BasinBlockEntity))
	// 		return Optional.empty();
	// 	return Optional.of((BasinBlockEntity) basinBE);
	// }

    // 	protected Optional<CreateAdvancement> getProcessedRecipeTrigger() {
	// 	return Optional.empty();
	// }

	// protected abstract boolean matchStaticFilters(RecipeHolder<? extends Recipe<?>> recipe);

	// protected abstract Object getRecipeCacheKey();
};