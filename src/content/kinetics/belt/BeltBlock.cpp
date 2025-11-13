#include "BeltBlock.hpp"
#include "content/kinetics/belt/BeltBlockEntity.hpp"
#include "AllBlocks.hpp"

std::shared_ptr<BlockActor> BeltBlock::newBlockEntity(const BlockPos &pos, const Block &block) const {
    return std::make_shared<BeltBlockEntity>(pos, "BeltBlock");
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
            // be->setController(currentPos);
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