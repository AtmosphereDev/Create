#pragma once
#include <algorithm>
#include <amethyst/runtime/ModContext.hpp>
#include <mc/src-client/common/client/gui/screens/controllers/ClientInstanceScreenController.hpp>
#include <mc/src-client/common/client/gui/controls/renderers/MinecraftUICustomRenderer.hpp>
#include "porting/ClientInputs.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsBehaviour.hpp"
#include "foundation/blockEntity/behaviour/ValueSettingsBoard.hpp"
#include <mc/src/common/locale/I18n.hpp>
#include "catnip/gui/UIRenderHelper.hpp"
#include "foundation/gui/AllGuiTextures.hpp"
#include <mc/src-client/common/client/gui/gui/GuiData.hpp>
#include "foundation/blockEntity/behaviour/ValueSettingsPacket.hpp"
#include <mc/src/common/network/LoopbackPacketSender.hpp>
#include "AllSoundEvents.hpp"
#include <mc/src-client/common/client/player/LocalPlayer.hpp>

class ValueSettingsScreen : public ClientInstanceScreenController {
public:
    static ValueSettingsScreen* instance;

private:
    bool initialized = false;
    int ticksOpen;
	ValueSettingsBoard board;
	int maxLabelWidth;
	int valueBarWidth;
	BlockPos pos;
	std::shared_ptr<ValueSettingsBehaviour::ValueSettings> initialSettings;
	ValueSettingsBehaviour::ValueSettings lastHovered;
	std::function<void(ValueSettingsBehaviour::ValueSettings&)> onHover;
	boolean iconMode;
	int milestoneSize;
	int soundCoolDown;
	int netId;

    int guiLeft = 0;
    int guiTop = 0;
    int windowWidth = 0;
    int windowHeight = 0;

public:
    ValueSettingsScreen(
        std::shared_ptr<ClientInstanceScreenModel> model, 
        const BlockPos& pos, 
        ValueSettingsBoard board, 
        std::shared_ptr<ValueSettingsBehaviour::ValueSettings> valueSettings, 
        std::function<void(ValueSettingsBehaviour::ValueSettings&)> onHover,
        int netId
    )
        : ClientInstanceScreenController(model), lastHovered(-1, -1), board(std::move(board)), pos(pos), initialSettings(valueSettings), 
                onHover(std::move(onHover)), netId(netId)
        {
            instance = this;
            // this->iconMode = board.formatter() instanceof ScrollOptionSettingsFormatter;
            this->iconMode = false; // TODO: implement ScrollOptionSettingsFormatter check
            this->milestoneSize = iconMode ? 8 : 4;
        }

    virtual void onDelete() override {
        if (instance == this) {
            instance = nullptr;
        }
        
        ClientInstanceScreenController::onDelete();
    }

    void setWindowSize(int windowWidth, int windowHeight) {
        this->windowWidth = windowWidth;
        this->windowHeight = windowHeight;
        this->guiLeft = (-windowWidth) / 2;
        this->guiTop = (-windowHeight) / 2;
    }

    virtual void initialize(MinecraftUIRenderContext& ctx) {   
        initialized = true;
        int maxValue = board.maxValue;
        maxLabelWidth = 0;
        int milestoneCount = maxValue / board.milestoneInterval + 1;
        int scale = maxValue > 128 ? 1 : 2;

        for (const auto& component : board.rows) {
            int w = ctx.getLineLength(*ctx.mDebugTextFontHandle.mDefaultFont, component, 1.0f, false);
            maxLabelWidth = std::max(maxLabelWidth, w);
        }
        Log::Info("maxLabelWidth {}", maxLabelWidth);

        if (iconMode) 
            maxLabelWidth = -18;

        valueBarWidth = (maxValue + 1) * scale + 1 + milestoneCount * milestoneSize;
        int width = (maxLabelWidth + 14) + (valueBarWidth + 10);
        int height = board.rows.size() * 11;

        setWindowSize(width, height);
    }

    virtual ui::DirtyFlag tick() override {
        ticksOpen++;    
        if (soundCoolDown > 0) soundCoolDown--;

        if (!ClientInputs::useKeyDown) {
            Log::Info("ValueSettingsScreen: Use key released, closing screen.");
            saveAndClose();
        }

        return ClientInstanceScreenController::tick();
    }

    void render(MinecraftUIRenderContext& ctx, ClientInstance& client, UIControl& owner, int32_t pass, RectangleArea& renderAABB, float alpha) {
        if (!initialized) initialize(ctx);
        float startTextAlpha = ctx.mTextAlpha;
        ctx.setTextAlpha(alpha);

        mce::Color whiteFlush = mce::Color(1.0f, 1.0f, 1.0f, alpha);

        mce::Color fontColor = 0x442000FF;
        fontColor.a = alpha;

        Vec2 clientScreenSize = client.mGuiData->clientUIScreenSize;
		float guiScale = client.mGuiData->mGuiScale;
        
        guiLeft = (int)((clientScreenSize.x - windowWidth) / 2.0f);
        guiTop = (int)((clientScreenSize.y  - windowHeight) / 2.0f);

        int x = guiLeft;
        int y = guiTop;

        int milestoneCount = board.maxValue / board.milestoneInterval + 1;
        int scale = board.maxValue > 128 ? 1 : 2;

        std::string title = i18n(board.title);
        std::string tip = i18n("gui.value_settings.release_to_confirm");
        double fadeIn = std::pow(std::clamp((ticksOpen + 0.0) / 4.0, 0.0, 1.0), 1.0);

        Font& font = *client.mMinecraftGame->mFontHandle.mDefaultFont;

        float titleWidth = ctx.getLineLength(font, title, 1.0f, false);
        float tipWidth = ctx.getLineLength(font, tip, 1.0f, false);

        int fattestLabel = std::max(titleWidth, tipWidth);

        if (iconMode) {
            // todo
        }

        int fatTipOffset = std::max(0, fattestLabel + 10 - (windowWidth + 13)) / 2;
        int bgWidth = std::max((windowWidth + 13), (fattestLabel + 10));
        int fadeInWidth = (int)(bgWidth * fadeIn);
        int fadeInStart = (bgWidth - fadeInWidth) / 2 - fatTipOffset;
        int additionalHeight = iconMode ? 46 : 33;

        // idk what this bit is supposed to do, the texture is UV'ed to nothing from what i can tell
        // UIRenderHelper::drawStretched(ctx, x - 11 + fadeInStart, y - 17, fadeInWidth, windowHeight + additionalHeight, AllGuiTextures::VALUE_SETTINGS_OUTER_BG);
        // UIRenderHelper::drawStretched(ctx, x - 10 + fadeInStart, y - 18, fadeInWidth - 2, 1, AllGuiTextures::VALUE_SETTINGS_OUTER_BG);
        // UIRenderHelper::drawStretched(ctx, x - 10 + fadeInStart, y - 17 + windowHeight + additionalHeight, fadeInWidth - 2, 1, AllGuiTextures::VALUE_SETTINGS_OUTER_BG);
        // ctx.flushImages(mce::Color::WHITE, 1.0f, "ui_flush");

        // Skipping this check for now, seems to make it worse here?
        if (fadeInWidth > fattestLabel) {
        int textX = x - 11 - fatTipOffset + bgWidth / 2;
            ctx.drawText(font, title, textX - titleWidth / 2, y - 14, 0xddddddff, alpha);
            ctx.drawText(font, tip, textX - tipWidth / 2, y + windowHeight + additionalHeight - 27, 0xaaaaaaff, alpha);
        }
        // else {
        //     Log::Info("Skipping title/tip render, fadeInWidth {} <= fattestLabel {}", fadeInWidth, fattestLabel);
        // }

        renderBrassFrame(ctx, x + maxLabelWidth + 14, y - 3, valueBarWidth + 8, board.rows.size() * 11 + 5);
        ctx.flushImages(whiteFlush, alpha, "ui_flush");

        UIRenderHelper::drawStretched(ctx, x + maxLabelWidth + 17, y, valueBarWidth + 2, board.rows.size() * 11 - 1, AllGuiTextures::VALUE_SETTINGS_BAR_BG);
        ctx.flushImages(whiteFlush, alpha, "ui_flush");

        int originalY = y;
        for (const auto& component : board.rows) {
            int valueBarX = x + maxLabelWidth + 14 + 4;

            if (!iconMode) {
                UIRenderHelper::drawCropped(ctx, x - 4, y, maxLabelWidth + 8, 11, AllGuiTextures::VALUE_SETTINGS_LABEL_BG);
                ctx.flushImages(whiteFlush, alpha, "ui_flush");

                for (int w = 0; w < valueBarWidth; w += AllGuiTextures::VALUE_SETTINGS_BAR.width() - 1) {
                    UIRenderHelper::drawCropped(ctx, valueBarX + w, y + 1, 
                        std::min(AllGuiTextures::VALUE_SETTINGS_BAR.width() - 1, valueBarWidth - w), 8,
                        AllGuiTextures::VALUE_SETTINGS_BAR);
                        ctx.flushImages(whiteFlush, alpha, "ui_flush");
                }

                ctx.drawText(font, component, x, y + 1, fontColor, alpha);
                ctx.flushText(alpha);
            }

            int milestoneX = valueBarX;
            for (int milestone = 0; milestone < milestoneCount; milestone++) {
                if (iconMode)
                    UIRenderHelper::render(ctx, milestoneX, y + 1, AllGuiTextures::VALUE_SETTINGS_WIDE_MILESTONE);
                else
                    UIRenderHelper::render(ctx, milestoneX, y + 1, AllGuiTextures::VALUE_SETTINGS_MILESTONE);
                milestoneX += milestoneSize + board.milestoneInterval * scale;
            }
            
            y += 11;
        }

        if (!iconMode) {
            renderBrassFrame(ctx, x - 7, originalY - 3, maxLabelWidth + 14, board.rows.size() * 11 + 5);
        }

        //if (ticksOpen < 1) {
        // ctx.setTextAlpha(startTextAlpha);
        // return;
        //}

		ValueSettingsBehaviour::ValueSettings closest = getClosestCoordinate(ClientInputs::mouseX / guiScale, ClientInputs::mouseY / guiScale);
        if (closest != lastHovered) {
            onHover(closest);
            if (soundCoolDown == 0) {
                // float pitch = (closest.value()) / (float)(board.maxValue());
                // pitch = Mth.lerp(pitch, 1.15f, 1.5f);
                // minecraft.getSoundManager()
                //     .play(SimpleSoundInstance.forUI(AllSoundEvents.SCROLL_VALUE.getMainEvent(), pitch, 0.25F));
                // ScrollValueHandler.wrenchCog.bump(3, -(closest.value() - lastHovered.value()) * 10);

                float pitch = (closest.value)/(float)(board.maxValue);
                pitch = std::lerp(1.15f, 1.5f, pitch);

                LocalPlayer& localPlayer = *client.getLocalPlayer();

                AllSoundEvents::SCROLL_VALUE.play(*localPlayer.getLevel(), *localPlayer.getPosition(), 0.25f, pitch); 
                soundCoolDown = 1;
            }
        }
        lastHovered = closest;

		Vec2 coordinate = getCoordinateOfValue(closest.row, closest.value);
		std::string cursorText = board.formatter.format(closest);

        //AllIcons cursorIcon = null;
        //if (board.formatter() instanceof ScrollOptionSettingsFormatter sosf)
        //    cursorIcon = sosf.getIcon(closest);

        //int cursorWidth = ((cursorIcon != null ? 16 : font.width(cursorText)) / 2) * 2 + 3;
        int cursorWidth = (ctx.getLineLength(font, cursorText, 1.0f, false) / 2) * 2 + 3;
		int cursorX = (int)coordinate.x - cursorWidth / 2;
		int cursorY = (int)coordinate.y - 7;
        
        /*if (cursorIcon != null) {
            AllGuiTextures.VALUE_SETTINGS_CURSOR_ICON.render(graphics, cursorX - 2, cursorY - 3);
            RenderSystem.setShaderColor(0.265625f, 0.125f, 0, 1);
            cursorIcon.render(graphics, cursorX + 1, cursorY - 1);
            RenderSystem.setShaderColor(1, 1, 1, 1);
            if (fadeInWidth > fattestLabel)
                graphics.drawString(font, cursorText, x - 11 - fatTipOffset + (bgWidth - font.width(cursorText)) / 2,
                    originalY + windowHeight + additionalHeight - 40, 0xFBDC7D, false);
            return;
        }*/

        UIRenderHelper::render(ctx, cursorX - 3, cursorY, AllGuiTextures::VALUE_SETTINGS_CURSOR_LEFT);
		UIRenderHelper::drawCropped(ctx, cursorX, cursorY, cursorWidth, 14, AllGuiTextures::VALUE_SETTINGS_CURSOR);
		UIRenderHelper::render(ctx, cursorX + cursorWidth, cursorY, AllGuiTextures::VALUE_SETTINGS_CURSOR_RIGHT);
		ctx.flushImages(whiteFlush, alpha, "ui_flush");

        ctx.drawText(font, cursorText, cursorX + 2, cursorY + 3, fontColor, alpha);
        ctx.flushText(alpha);

        ctx.setTextAlpha(startTextAlpha);
    }

    ValueSettingsBehaviour::ValueSettings getClosestCoordinate(int mouseX, int mouseY) {
        int row = 0;
        int column = 0;
        bool milestonesOnly = ClientInputs::shiftKeyDown;

        double bestDiff = DBL_MAX;
        for (; row < board.rows.size(); row++) {
            Vec2 coord = getCoordinateOfValue(row, 0);
            double diff = std::abs(coord.y - mouseY);
            if (bestDiff < diff)
                break;
            bestDiff = diff;
        }
        row -= 1;

        bestDiff = DBL_MAX;
        for (; column <= board.maxValue; column++) {
            Vec2 coord = getCoordinateOfValue(row, milestonesOnly ? column * board.milestoneInterval : column);
            double diff = std::abs(coord.x - mouseX);
            if (bestDiff < diff)
                break;
            bestDiff = diff;
        }
        column -= 1;

        return ValueSettingsBehaviour::ValueSettings(
            row,
            milestonesOnly ? std::min(column * board.milestoneInterval, board.maxValue) : column
        );
    }

    void renderBrassFrame(MinecraftUIRenderContext& ctx, int x, int y, int w, int h) {
        UIRenderHelper::render(ctx, x, y, AllGuiTextures::BRASS_FRAME_TL);
        UIRenderHelper::render(ctx, x + w - 4, y, AllGuiTextures::BRASS_FRAME_TR);
        UIRenderHelper::render(ctx, x, y + h - 4, AllGuiTextures::BRASS_FRAME_BL);
        UIRenderHelper::render(ctx, x + w - 4, y + h - 4, AllGuiTextures::BRASS_FRAME_BR);

        if (h > 8) {
            UIRenderHelper::drawStretched(ctx, x, y + 4, 3, h-8, AllGuiTextures::BRASS_FRAME_LEFT);
            UIRenderHelper::drawStretched(ctx, x + w - 3, y + 4, 3, h-8, AllGuiTextures::BRASS_FRAME_RIGHT);
        }

        if (w > 8) {
            UIRenderHelper::drawStretched(ctx, x + 4, y, w-8, 3, AllGuiTextures::BRASS_FRAME_TOP);
            UIRenderHelper::drawStretched(ctx, x + 4, y + h - 3, w-8, 3, AllGuiTextures::BRASS_FRAME_BOTTOM);
        }
    }

    Vec2 getCoordinateOfValue(int row, int column) {
        int scale = board.maxValue > 128 ? 1 : 2;
        float xOut =
            guiLeft + ((std::max(1, column) - 1) / board.milestoneInterval) * milestoneSize + column * scale + 1.5f;
        xOut += maxLabelWidth + 14 + 4;

        if (column % board.milestoneInterval == 0)
            xOut += milestoneSize / 2;
        if (column > 0)
            xOut += milestoneSize;

        float yOut = guiTop + (row + .5f) * 11 - 0.5f;
        return Vec2(xOut, yOut);
    }

    void saveAndClose() {
        const ClientInstance& ci = *Amethyst::GetClientCtx().mClientInstance;
        float guiScale = ci.mGuiData->mGuiScale;
        ValueSettingsBehaviour::ValueSettings current = getClosestCoordinate(ClientInputs::mouseX / guiScale, ClientInputs::mouseY / guiScale);

        auto& networking = Amethyst::GetNetworkManager();

        // FIXME: value settings may be face-sensitive on future components
        // ^ this was in original src
        networking.SendToServer(
            *ci.mPacketSender.get(),
            std::make_unique<ValueSettingsPacket>(pos, current.row, current.value, FacingID::NORTH, false, netId)
        );

        this->tryExit();
    }

    static void AddEventListeners();
};

class ValueSettingsScreenRenderer : public MinecraftUICustomRenderer {
public:
    virtual std::shared_ptr<UICustomRenderer> clone() const override {
        return std::make_shared<ValueSettingsScreenRenderer>();
    }

    virtual void render(MinecraftUIRenderContext& ctx, IClientInstance& client, UIControl& owner, int32_t pass, RectangleArea& renderAABB) override {
        ValueSettingsScreen* screen = ValueSettingsScreen::instance;
        if (!screen) return;
        screen->render(ctx, (ClientInstance&)client, owner, pass, renderAABB, this->mPropagatedAlpha);
    }
};