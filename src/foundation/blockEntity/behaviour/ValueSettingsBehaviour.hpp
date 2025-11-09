#pragma once
#include <string>
#include "content/equipment/clipboard/ClipboardCloneable.hpp"
#include "foundation/blockEntity/behaviour/ValueBoxTransform.hpp"

class Player;
class HitResult;
class ItemStack;
class BlockEntityBehaviour;
class ValueSettingsBoard;

class ValueSettingsBehaviour : public ClipboardCloneable {
public:
    class ValueSettings {
    public:
        int row;
        int value;

        ValueSettings(int row, int value) 
            : row(row), value(value) {}

        std::string format() const {
            return std::to_string(value);
        }

        bool operator==(const ValueSettings& other) const {
            return row == other.row && value == other.value;
        }

        bool operator!=(const ValueSettings& other) const {
            return !(*this == other);
        }
    };

    virtual bool testHit(const Vec3& hit) = 0;

    virtual bool isActive() const = 0;

    virtual bool onlyVisibleWithWrench() const {
        return false;
    }

    virtual void newSettingHovered(ValueSettings& valueSetting) {}

    virtual std::shared_ptr<ValueBoxTransform> getSlotPositioning() const = 0;

    virtual ValueSettingsBoard createBoard(const Player& player, const HitResult& hit) = 0;

    virtual void setValueSettings(const Player& player, std::shared_ptr<ValueSettings> valueSetting, bool ctrlDown) = 0;

    virtual std::shared_ptr<ValueSettings> getValueSettings() = 0;

    virtual bool acceptsValueSettings() const {
        return true;
    }

    virtual std::string getClipboardKey() const override {
        return "Settings";
    }

    virtual bool writeToClipboard(CompoundTag& tag, FacingID side) override {
        if (!acceptsValueSettings())
            return false;

        std::shared_ptr<ValueSettings> settings = getValueSettings();
        tag.putInt("Row", settings->row);
        tag.putInt("Value", settings->value);
        return true;
    }

    virtual bool readFromClipboard(const CompoundTag& tag, const Player& player, FacingID side, bool simulate) override {
        if (!acceptsValueSettings())
            return false;

        if (!tag.contains("Value") || !tag.contains("Row"))
            return false;

        if (simulate) 
            return true;

        setValueSettings(player, std::make_shared<ValueSettings>(tag.getInt("Row"), tag.getInt("Value")), false);
        return true;
    }

    virtual void playFeedbackSound(BlockEntityBehaviour& origin) {
        // origin.getWorld()
		// 	.playSound(null, origin.getPos(), SoundEvents.ITEM_FRAME_ADD_ITEM, SoundSource.BLOCKS, 0.25f, 2f);
		// origin.getWorld()
		// 	.playSound(null, origin.getPos(), SoundEvents.NOTE_BLOCK_IRON_XYLOPHONE.value(), SoundSource.BLOCKS, 0.03f,
		// 		1.125f);
    }

    virtual void onShortInteract(const Player& player, FacingID side, const HitResult& hit) {}

    virtual bool bypassesInput(const ItemStack& stack) const {
        return false;
    }

    virtual bool mayInteract(const Player& player) const {
        return true;
    }

    virtual int netId() const {
        return 0;
    }
};