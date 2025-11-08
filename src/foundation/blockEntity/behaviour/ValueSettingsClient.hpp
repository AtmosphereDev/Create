#pragma once
#include <amethyst/Imports.hpp> 
#include <amethyst/runtime/ModContext.hpp>
#include <amethyst/runtime/events/BlockEvents.hpp>
#include <mc/src-client/common/client/game/ClientInstance.hpp>

class BehaviourType;

class ValueSettingsClient {
public:
    ClientInstance* mc = nullptr;
    int interactHeldTicks = -1;
    std::optional<BlockPos> interactHeldPos;
    std::optional<const BehaviourType*> interactHeldBehaviour;
    std::optional<FacingID> interactHeldFace;

    std::vector<std::string> lastHoverTip;
    int hoverTicks;
    int hoverWarmup;

	ValueSettingsClient() : hoverTicks(0), hoverWarmup(0) {
        
    }

    void cancelIfWarmupAlreadyStarted(BeforeBlockUseEvent& event) {
        if (interactHeldTicks != -1 && event.pos == interactHeldPos) {
            event.Cancel();
        }
    }

    void startInteractionWith(const BlockPos& pos, const BehaviourType& type, FacingID direction);

    void cancelInteraction() {
        interactHeldTicks = -1;
        Log::Info("CancelInteraction called!");
    }

    void tick();
};