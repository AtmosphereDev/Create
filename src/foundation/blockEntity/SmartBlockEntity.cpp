#include "SmartBlockEntity.hpp"
#include "foundation/blockEntity/behaviour/BlockEntityBehaviour.hpp"

void SmartBlockEntity::afterConstructed()
{
    CachedRenderBBBlockEntity::afterConstructed();
    std::vector<std::shared_ptr<BlockEntityBehaviour>> list;

    addBehaviours(list);

    for (auto& behaviour : list) {
        addBehaviour(behaviour->getType(), behaviour);
    }
}

void SmartBlockEntity::tick(BlockSource &source)
{
    if (!initialized) {
        initialize();
        initialized = true;
    }

    if (--lazyTickCounter <= 0) {
        lazyTickCounter = lazyTickRate;
        lazyTick();
    }

    forEachBehaviour([](const std::shared_ptr<BlockEntityBehaviour>& b) { 
        b->tick(); 
    });

    CachedRenderBBBlockEntity::tick(source);
}

void SmartBlockEntity::initialize()
{
    if (firstNbtRead) {
        firstNbtRead = false;
        // NeoForge.EVENT_BUS.post(new BlockEntityBehaviourEvent(this, behaviours)); 
        // TODO: ^
    }

    forEachBehaviour([](const std::shared_ptr<BlockEntityBehaviour>& b) { 
        b->initialize(); 
    });
    lazyTick();
}

void SmartBlockEntity::invalidate()
 {
    forEachBehaviour([](const std::shared_ptr<BlockEntityBehaviour>& b) { 
        b->unload(); 
    });
}

void SmartBlockEntity::destroy()
{
    forEachBehaviour([](const std::shared_ptr<BlockEntityBehaviour>& b) { 
        b->destroy(); 
    });
}