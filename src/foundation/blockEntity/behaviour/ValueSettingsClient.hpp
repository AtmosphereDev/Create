#pragma once
#include <amethyst/Imports.hpp> 
#include <amethyst/runtime/ModContext.hpp>
#include <amethyst/runtime/events/BlockEvents.hpp>
#include <mc/src-client/common/client/game/ClientInstance.hpp>
#include <mc/src-client/common/client/game/MinecraftGame.hpp>
#include <mc/src-client/common/client/gui/controls/renderers/MinecraftUICustomRenderer.hpp>
#include <mc/src-client/common/client/gui/gui/GuiData.hpp>
#include <algorithm>
#include <mc/src/common/locale/I18n.hpp>
#include <mc/src-client/common/client/gui/screens/SceneStack.hpp>

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

    void showHoverTip(std::vector<std::string> tip) {
        if (hoverWarmup < 6) {
            hoverWarmup += 2;
            return;
        }
        else {
            hoverWarmup++;
        }

        hoverTicks = hoverTicks == 0 ? 11 : std::max(hoverTicks, 6);
        lastHoverTip = std::move(tip);
    }

    void render(MinecraftUIRenderContext& ctx, ClientInstance& client, UIControl& owner, int32_t pass, RectangleArea& renderAABB, float propagatedAlpha) {
  //      std::string screenName = client.mSceneStack->getScreenName();
		//Log::Info("ValueSettingsClient::render called on screen {}", screenName);

        if (hoverTicks == 0 || lastHoverTip.empty())
            return;

        Vec2 screenSize = client.mGuiData->clientUIScreenSize;
        int x = screenSize.x / 2;
        int y = screenSize.y - 75 - lastHoverTip.size() * 12;
        float alpha = hoverTicks > 5 ? (11 - hoverTicks) / 5.0f : std::min(1.0f, hoverTicks / 5.0f);

        mce::Color color = 0xFFFFFFFF;
        mce::Color titleColor = 0xFBDC7DFF;
        color.a = alpha;
        titleColor.a = alpha;

        Font& font = *client.mMinecraftGame->mFontHandle.mDefaultFont;

        for (int i = 0; i < lastHoverTip.size(); i++) {
            std::string line = i18n(lastHoverTip[i]);
            int width = ctx.getLineLength(font, line, 1.0f, false);
            ctx.drawText(font, line, x - width / 2, y, (i == 0 ? titleColor : color), alpha);
            y += 12;
        }
    }

    void tick();

    static void AddEventListeners();
};

class ValueSettingsClientRenderer : public MinecraftUICustomRenderer {
public:
    virtual std::shared_ptr<UICustomRenderer> clone() const override {
        return std::make_shared<ValueSettingsClientRenderer>();
    }

    virtual void render(MinecraftUIRenderContext& ctx, IClientInstance& client, UIControl& owner, int32_t pass, RectangleArea& renderAABB) override;
};