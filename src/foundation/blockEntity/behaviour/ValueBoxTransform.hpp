#pragma once
#include <amethyst/Imports.hpp>
#include <mc/src/common/world/level/BlockSource.hpp>
#include <mc/src-deps/renderer/MatrixStack.hpp>
#include <mc/src/common/world/level/block/Block.hpp>
#include <mc/src/common/world/level/block/VanillaStates.hpp>

class ValueBoxTransform {
public:
    virtual std::optional<Vec3> getLocalOffset(const BlockSource& region, const BlockPos& pos, const Block& state) const = 0;

    virtual void rotate(const BlockSource& region, const BlockPos& pos, const Block& state, MatrixStack& ms) const = 0;

    virtual bool testHit(const BlockSource& region, const BlockPos& pos, const Block& state, const Vec3& localHit) const {
        std::optional<Vec3> offset = getLocalOffset(region, pos, state);
        if (!offset.has_value()) return false;
        return localHit.distance(offset.value()) < getScale() / 2;
    }

    void transform(const BlockSource& region, const BlockPos& pos, const Block& state, MatrixStack& ms) const {
        std::optional<Vec3> position = getLocalOffset(region, pos, state);
        if (!position.has_value()) 
            return;

        auto& mat = ms.getTop();
        mat.translate(position.value());    
        rotate(region, pos, state, ms);
        mat.scale(getScale());
    }

    virtual bool shouldRender(const BlockSource& region, const BlockPos& pos, const Block& state) const {
        return !state.isAir() && getLocalOffset(region, pos, state).has_value();
    }

    virtual int getOverrideColor() const {
        return -1;
    }

protected:
    Vec3 rotateHorizontally(const Block& state, const Vec3& vec) const {
        float yRot = 0;

        if (state.hasState(VanillaStates::FacingDirection)) {
            yRot = Facing::getYAngle(state.getState<FacingID>(VanillaStates::FacingDirection));
        }

        // todo update when using
        // if (state.hasProperty(BlockStateProperties.HORIZONTAL_FACING))
		// 	yRot = AngleHelper.horizontalAngle(state.getValue(BlockStateProperties.HORIZONTAL_FACING)); 

        return Vec3::rotateCentered(vec, yRot, Facing::Axis::Y);
    }

public:
    float getScale() const {
        return 0.5f;
    }

    float getFontScale() const {
        return 1.0f / 64.0f;
    }
};

// Had to move outside for c++
class DualValueBoxTransform : public ValueBoxTransform {
protected:
    bool first;

public:
    DualValueBoxTransform(bool first) : first(first) {}

    bool isFirst() const {
        return first;
    }

    static std::pair<std::shared_ptr<ValueBoxTransform>, std::shared_ptr<ValueBoxTransform>> 
        makeSlots(const std::function<std::shared_ptr<DualValueBoxTransform>(bool)>& factory) 
    {
        return { factory(true), factory(false) };
    }

    virtual bool testHit(const BlockSource& region, const BlockPos& pos, const Block& state, const Vec3& localHit) const override {
        std::optional<Vec3> offset = getLocalOffset(region, pos, state);
        if (!offset.has_value()) return false;
        return localHit.distance(offset.value()) < getScale() / 3.5f;
    }
};

class SidedValueBoxTransform : public ValueBoxTransform {
protected:
    FacingID direction = FacingID::NORTH;

public:
    SidedValueBoxTransform& fromSide(FacingID dir) {
        direction = dir;
        return *this;
    }

    virtual std::optional<Vec3> getLocalOffset(const BlockSource& region, const BlockPos& pos, const Block& state) const override {
        Vec3 location = getSouthLocation();
        location.rotateCentered(Facing::getYAngle(getSide()), Facing::Axis::Y);
        location.rotateCentered(Facing::getXAngle(getSide()), Facing::Axis::X);
        return location;
    }

protected:
    virtual Vec3 getSouthLocation() const = 0;

public:
    virtual void rotate(const BlockSource& region, const BlockPos& pos, const Block& state, MatrixStack& ms) const override {
        auto& mat = ms.getTop();
        float yRot = Facing::getYAngle(getSide()) + 180.0f;
        float xRot = getSide() == FacingID::UP ? 90.0f : (getSide() == FacingID::DOWN ? 270.0f : 0.0f);
        mat.rotateYDegrees(yRot);
        mat.rotateXDegrees(xRot);
    }

    virtual bool shouldRender(const BlockSource& region, const BlockPos& pos, const Block& state) const override {
        return ValueBoxTransform::shouldRender(region, pos, state) && isSideActive(state, getSide());
    }

    virtual bool testHit(const BlockSource& region, const BlockPos& pos, const Block& state, const Vec3& localHit) const override {
        return isSideActive(state, getSide()) && ValueBoxTransform::testHit(region, pos, state, localHit);
    }

    bool isSideActive(const Block& state, FacingID side) const {
        return true;
    }

public:
    FacingID getSide() const {
        return direction;
    }
}; 