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

    ValueSettingsClient() {
        mc = Amethyst::GetClientCtx().mClientInstance;
    }

    void cancelIfWarmupAlreadyStarted(BeforeBlockUseEvent& event) {
        if (interactHeldTicks != -1 && event.pos == interactHeldPos) {
            event.Cancel();
        }
    }

    void startInteractionWith(const BlockPos& pos, const BehaviourType& type, FacingID direction) {
        interactHeldTicks = 0;
        interactHeldPos = pos;
        interactHeldBehaviour = &type;
        interactHeldFace = direction;
    }

    void cancelInteraction() {
        interactHeldTicks = -1;
    }

    void tick();
};