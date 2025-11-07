#pragma once
#include <mc/src/common/world/Facing.hpp>

#include <mc/src/common/nbt/CompoundTagVariant.hpp>
#include <mc/src/common/nbt/CompoundTag.hpp>

class CompoundTag;
class HashedString;
class Player;

class ClipboardCloneable {
public:
    virtual std::string getClipboardKey() const = 0;

    virtual bool writeToClipboard(CompoundTag& tag, FacingID side) = 0;

    virtual bool readFromClipboard(const CompoundTag& tag, const Player& player, FacingID side, bool simulate) = 0;
};