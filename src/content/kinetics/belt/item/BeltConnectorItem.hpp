#pragma once
#include <mc/src/common/world/item/Item.hpp>
#include <mc/src/common/world/actor/player/Player.hpp>
#include <mc/src/common/world/level/BlockSource.hpp>
#include <mc/src/common/world/level/block/Block.hpp>
#include "content/kinetics/belt/BeltBlock.hpp"
#include <mc/src/common/world/level/block/VanillaStates.hpp>
#include "content/kinetics/base/KineticBlockEntity.hpp"
#include "AllBlocks.hpp"
#include "infrastructure/config/AllConfigs.hpp"
#include "content/kinetics/simpleRelays/ShaftBlock.hpp"

// original mod seems to inherit from BlockItem but seems unnecessary?
class BeltConnectorItem : public Item {
public:
    BeltConnectorItem(const std::string& name, short id)
    : Item(name, id) {
        setIconInfo("fx_create:belt_connector", 0);
    }

    virtual InteractionResult _useOn(ItemStack& heldStack, Actor& playerEntity, BlockPos pos, FacingID face, const Vec3& hit) const override {
        Player& player = (Player&)playerEntity;

        // if (sneaking) {
        //     heldStack.remove(AllDataComponents.BELT_FIRST_SHAFT);
        //     return InteractionResult::Result::SUCCESS;
        // }

        Dimension& dim = playerEntity.getDimension();
        bool validAxis = validateAxis(dim, pos);

        if (dim.isClientSide()) {
            return validAxis ? InteractionResult(InteractionResult::Result::SUCCESS | InteractionResult::Result::SWING) : InteractionResult();
        }

        std::optional<BlockPos> firstPulley = std::nullopt;

        // Remove first if no longer existant or valid
        if (heldStack.mUserData != nullptr && heldStack.mUserData->contains("BeltFirstShaft")) {
            firstPulley = NbtUtils::readBlockPos(*heldStack.mUserData->getCompound("BeltFirstShaft"));

            if (!validateAxis(dim, *firstPulley) || !firstPulley->closerThan(pos, maxLength())) {
                heldStack.mUserData->remove("BeltFirstShaft");
            }
        }

        if (!validAxis) 
            return InteractionResult();

        if (heldStack.mUserData != nullptr && heldStack.mUserData->contains("BeltFirstShaft")) {
            if (!canConnect(dim, *firstPulley, pos)) {
                return InteractionResult();
            }

            if (*firstPulley != pos) {
                createBelts(dim, firstPulley.value(), pos);
                if (!player.isCreative()) heldStack.mCount--;
            }
            return InteractionResult(InteractionResult::Result::SUCCESS);
        }

        if (heldStack.mUserData == nullptr) {
            heldStack.mUserData = new CompoundTag();
        }
        heldStack.mUserData->put("BeltFirstShaft", NbtUtils::writeBlockPos(pos));
        return InteractionResult(InteractionResult::Result::SUCCESS);
    }

    static void createBelts(Dimension& world, const BlockPos& start, const BlockPos& end) {
        // impl
        BlockSource& region = *world.mBlockSource;

        // world.playSound(null, BlockPos.containing(VecHelper.getCenterOf(start.offset(end))
		// 	.scale(.5f)), SoundEvents.WOOL_PLACE, SoundSource.BLOCKS, 0.5F, 1F);

        BeltSlope::Type slope = getSlopeBetween(start, end);
        FacingID facing = getFacingFromTo(start, end);

        BlockPos diff = end - start;
        if (diff.x == diff.z)
            facing = Facing::fromDirectionAndAxis(Facing::getAxisDirection(facing), 
                region.getBlock(start).getState<Facing::Axis>(VanillaStates::PillarAxis) == Facing::Axis::X ? Facing::Axis::Z : Facing::Axis::X);

        std::vector<BlockPos> beltsToCreate = getBeltChainBetween(start, end, slope, facing);
        const Block& beltBlock = *AllBlocks::BELT->mDefaultState;
        bool failed = false;

        for (const BlockPos& pos : beltsToCreate) {
            const Block& existingBlock = region.getBlock(pos);
            if (existingBlock.mLegacyBlock->mDestroySpeed == -1 && !existingBlock.isAir()) {
                failed = true;
                break;
            }

            BeltPart::Type part = pos == start ? BeltPart::START : pos == end ? BeltPart::END : BeltPart::MIDDLE;
            const Block& shaftState = region.getBlock(pos);
            bool pulley = ShaftBlock::isShaft(shaftState);
            if (part == BeltPart::MIDDLE && pulley)
                part = BeltPart::PULLEY;
            if (pulley && shaftState.getState<Facing::Axis>(VanillaStates::PillarAxis) == Facing::Axis::Y)
                slope = BeltSlope::SIDEWAYS;

            if (!existingBlock.canBeBuiltOver(region, pos)) 
                world.destroyBlock(pos, false);

            KineticBlockEntity::switchToBlockState(world, pos,
                *AllBlocks::BELT->mDefaultState->setState(BeltBlock::SLOPE(), slope)
                    ->setState(BeltBlock::PART(), part)
                    ->setState(HorizontalKineticBlock::HORIZONTAL_FACING(), facing).get());
        }

        if (!failed)
            return;

        for (const BlockPos& pos : beltsToCreate) {
            if (AllBlocks::BELT == region.getBlock(pos).mLegacyBlock) {
                world.destroyBlock(pos, false);
            }
        }
    }

    static FacingID getFacingFromTo(const BlockPos& start, const BlockPos& end) {
        Facing::Axis beltAxis = start.x == end.x ? Facing::Axis::Z : Facing::Axis::X;
        BlockPos diff = end - start;
        Facing::AxisDirection axisDirection = Facing::AxisDirection::POSITIVE;

        if (diff.x == 0 && diff.z == 0)
            axisDirection = diff.y > 0 ? Facing::AxisDirection::POSITIVE : Facing::AxisDirection::NEGATIVE;
        else
            axisDirection =
                Facing::choose(beltAxis, diff.x, 0, diff.z) > 0 ? Facing::AxisDirection::POSITIVE : Facing::AxisDirection::NEGATIVE;

        return Facing::fromDirectionAndAxis(axisDirection, beltAxis);
    }

    static BeltSlope::Type getSlopeBetween(const BlockPos& start, const BlockPos& end) {
        BlockPos diff = end - start;

        if (diff.y != 0) {
            if (diff.z != 0 || diff.x != 0)
                return diff.y > 0 ? BeltSlope::UPWARD : BeltSlope::DOWNWARD;
            return BeltSlope::VERTICAL;
        }

        return BeltSlope::HORIZONTAL;
    }

    static std::vector<BlockPos> getBeltChainBetween(const BlockPos& start, const BlockPos& end, BeltSlope::Type slope, FacingID facing) {
        std::vector<BlockPos> positions;
        int limit = 1000;
        BlockPos current = start;

        do {
            positions.push_back(current);

            if (slope == BeltSlope::VERTICAL) {
                current = current.above(Facing::getAxisDirection(facing) == Facing::AxisDirection::POSITIVE ? 1 : -1);
                continue;
            }

            current = current.neighbor(facing);
            if (slope != BeltSlope::HORIZONTAL)
                current = current.above(slope == BeltSlope::UPWARD ? 1 : -1);

        } while (current != end && limit-- > 0);

        positions.push_back(end);
        return positions;
    }

    static bool canConnect(Dimension& dim, const BlockPos& first, const BlockPos& second) {
        BlockSource& region = *dim.mBlockSource;
        if (!region.areChunksFullyLoaded(first, 1) || !region.areChunksFullyLoaded(second, 1)) {
            return false;
        }
        if (!second.closerThan(first, maxLength())) 
            return false;

        BlockPos diff = second - first;
        Facing::Axis shaftAxis = region.getBlock(first)
            .getState<Facing::Axis>(VanillaStates::PillarAxis);

        int x = diff.x;
        int y = diff.y;
        int z = diff.z;
        int sames = ((std::abs(x) == std::abs(y)) ? 1 : 0) + ((std::abs(y) == std::abs(z)) ? 1 : 0) 
            + ((std::abs(z) == std::abs(x)) ? 1 : 0);

        if (Facing::choose(shaftAxis, x, y, z) != 0)
            return false;
        if (sames != 1) 
            return false;
        if (shaftAxis != region.getBlock(second).getState<Facing::Axis>(VanillaStates::PillarAxis))
            return false;
        if (shaftAxis == Facing::Axis::Y && diff.x != 0 && diff.z != 0)
            return false;

        KineticBlockEntity* blockEntity = KineticBlockEntity::asKineticBlockEntity(region.getBlockEntity(first));
        KineticBlockEntity* blockEntity2 = KineticBlockEntity::asKineticBlockEntity(region.getBlockEntity(second));

        if (blockEntity == nullptr || blockEntity2 == nullptr)
            return false;

        float speed1 = blockEntity->getTheoreticalSpeed();
        float speed2 = blockEntity2->getTheoreticalSpeed();

        if (mce::Math::signum(speed1) != mce::Math::signum(speed2) && speed1 != 0 && speed2 != 0)
            return false;

        BlockPos step = BlockPos::containing(mce::Math::signum(diff.x), mce::Math::signum(diff.y), mce::Math::signum(diff.z));
        int limit = 1000;
        for (BlockPos currentPos = first + step; currentPos != second && limit-- > 0; currentPos = currentPos + step) {
            const Block& blockState = region.getBlock(currentPos);
            if (AllBlocks::SHAFT == blockState.mLegacyBlock && blockState.getState<Facing::Axis>(VanillaStates::PillarAxis) == shaftAxis) {
                continue;
            }
            if (!blockState.canBeBuiltOver(region, currentPos)) 
                return false;
        }

        return true;
    }

    static uint64_t maxLength() {
        return AllConfigs::server().kinetics.maxBeltLength.get();
    }

    static bool validateAxis(const Dimension& dim, const BlockPos& pos) {
        if (!dim.mBlockSource->areChunksFullyLoaded(pos, 1))
            return false;
        if (AllBlocks::SHAFT != dim.mBlockSource->getBlock(pos).mLegacyBlock)
            return false;
        return true;
    }
};