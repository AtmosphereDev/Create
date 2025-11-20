#include "BeltBlock.hpp"
#include "content/kinetics/belt/BeltBlockEntity.hpp"
#include "AllBlocks.hpp"
#include "mc/src/common/world/actor/Actor.hpp"
#include "foundation/item/ItemHelper.hpp"

std::shared_ptr<BlockActor> BeltBlock::newBlockEntity(const BlockPos &pos, const Block &block) const {
    return std::make_shared<BeltBlockEntity>(pos, "BeltBlock");
}

void BeltBlock::entityInside(BlockSource &region, const BlockPos &pos, Actor &actor)
{
    const Block& state = region.getBlock(pos);
    if (!canTransportObjects(state)) {
        return;
    }

    // if (entityIn instanceof Player player) {
    //     if (player.isShiftKeyDown() && !AllItems.CARDBOARD_BOOTS.isIn(player.getItemBySlot(EquipmentSlot.FEET)))
    //         return;
    //     if (player.getAbilities().flying)
    //         return;
    // }

    // if (DivingBootsItem.isWornBy(entityIn))
    //     return;

    BeltBlockEntity* belt = BeltHelper::getSegmentBE(region, pos);
    if (belt == nullptr)
        return;

    ItemStack asItem = ItemHelper::fromItemEntity(actor);
    if (!asItem.isEmpty()) {
        if (region.isClientSide()) return;
        if (actor.getDeltaMovement().y > 0) return;

        // Vec3 targetLocation = VecHelper.getCenterOf(pos)
		// 		.add(0, 5 / 16f, 0);
		// 	if (!PackageEntity.centerPackage(entityIn, targetLocation))
		// 		return;
		// 	if (BeltTunnelInteractionHandler.getTunnelOnPosition(worldIn, pos) != null)
		// 		return;

        withBlockEntityDo(region, pos, [&](BeltBlockEntity& be) {
            IItemHandler* handler = be.getLevel().getCapability(Amethyst::GetServerCtx().mCapabilities.itemHandler.BLOCK, pos, state, &be, std::optional<FacingID>{});
            if (handler == nullptr) return;

            // if implementing packages, this needs to be changed!! TODO
            ItemActor& itemActor = static_cast<ItemActor&>(actor);

            ItemStack remainder = handler->insertItem(0, asItem, false);
            if (remainder.isEmpty())
                actor.remove();
            else if (remainder.mCount != asItem.mCount) {
                itemActor.mItem = remainder;
            }
        });

        return;
    }
}

AABB BeltBlock::getCollisionShape(const Block &block, const BlockSource &region, const BlockPos &at, optional_ref<const GetCollisionShapeInterface> shapeInterface) const
{
    // bad impl to test since i cba to do proper collisions 
    AABB base = BlockLegacy::getCollisionShape(block, region, at, shapeInterface);
    // chop a bit off the top
    base.max.y -= 0.1875f;
    return base;
}

void BeltBlock::initBelt(BlockSource &region, const BlockPos &pos)
{
    if (region.isClientSide())
        return;

    const Block& block = region.getBlock(pos);
    if (block.mLegacyBlock != AllBlocks::BELT) 
        return;

    int limit = 1000;
    BlockPos currentPos = pos;

    while (limit-- > 0) {
        const Block& currentState = region.getBlock(currentPos);
        if (currentState.mLegacyBlock != AllBlocks::BELT) {
            region.mLevel->destroyBlock(region, currentPos, true);
            return;
        }
        std::optional<BlockPos> nextSegementPos = nextSegmentPosition(currentState, currentPos, false);
        if (!nextSegementPos.has_value())
            break;
        if (region.areChunksFullyLoaded(nextSegementPos.value(), 1))
            return;
        currentPos = nextSegementPos.value(); 
    }

    // Init belts
    int index = 0;
    std::vector<BlockPos> beltChain = getBeltChain(region, currentPos);
    if (beltChain.size() < 2) {
        region.mDimension->destroyBlock(currentPos, true);
        return;
    }

    for (const BlockPos& beltPos : beltChain) {
        BlockActor* blockEntity = region.getBlockEntity(beltPos);
        const Block& currentState = region.getBlock(beltPos);

        BeltBlockEntity* be = dynamic_cast<BeltBlockEntity*>(blockEntity);
        if (be != nullptr && AllBlocks::BELT == currentState.mLegacyBlock) {
            be->setController(currentPos);
            be->beltLength = static_cast<int>(beltChain.size());
            be->index = index;
            be->attachKinetics();
            be->setChanged();
            be->sendData();

            // if (be->isController() && !canTransportObjects(currentState)) {
            //     be->getInventory()->ejectAll();
            // }
        }
        else {
            region.mDimension->destroyBlock(beltPos, true);
            return;
        }

        index++;
    }
}

std::vector<BlockPos> BeltBlock::getBeltChain(BlockSource &region, const BlockPos &startPos)
{
    std::vector<BlockPos> positions;
    
    const Block& blockState = region.getBlock(startPos);
    if (blockState.mLegacyBlock != AllBlocks::BELT) 
        return positions;
    
    int limit = 1000;
    std::optional<BlockPos> currentPos = startPos;
    while (limit-- > 0 && currentPos.has_value()) {
        const Block& currentState = region.getBlock(currentPos.value());
        if (currentState.mLegacyBlock != AllBlocks::BELT) {
            break;
        }
        positions.push_back(currentPos.value());
        currentPos = nextSegmentPosition(currentState, currentPos.value(), true);
    }

    if (positions.size() > 10) {
		Log::Error("Belt chain starting at {} is too long ({} segments), cancelling initialization!", startPos, positions.size());
		positions.clear();
		return positions;
    }

    return positions;
}

std::optional<BlockPos> BeltBlock::nextSegmentPosition(const Block &state, const BlockPos &_pos, bool forward)
{
    BlockPos pos = _pos;
    FacingID direction = state.getState<FacingID>(HORIZONTAL_FACING());
    BeltSlope::Type slope = state.getState<BeltSlope::Type>(SLOPE());
    BeltPart::Type part = state.getState<BeltPart::Type>(PART());

    int offset = forward ? 1 : -1;

    if (part == BeltPart::END && forward || part == BeltPart::START && !forward) {
        return std::nullopt;
    }

    if (slope == BeltSlope::VERTICAL) {
        return pos.above(Facing::getAxisDirection(direction) == Facing::AxisDirection::POSITIVE ? offset : -offset);
    }
    pos = pos.neighbor(direction, offset);
    if (slope != BeltSlope::HORIZONTAL && slope != BeltSlope::SIDEWAYS) {
        return pos.above(slope == BeltSlope::UPWARD ? offset : -offset);
    }
    return pos;
}

bool BeltBlock::canTransportObjects(const Block &state)
{
    if (AllBlocks::BELT != state.mLegacyBlock) {
        return false;
    }

    BeltSlope::Type slope = state.getState<BeltSlope::Type>(SLOPE());
    return slope != BeltSlope::SIDEWAYS && slope != BeltSlope::VERTICAL;
}
