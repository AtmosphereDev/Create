#include "CreativeMotorBlockEntity.hpp"
#include "content/kinetics/motor/CreativeMotorBlock.hpp"
#include "AllBlocks.hpp"

float CreativeMotorBlockEntity::getGeneratedSpeed() const
 {
    if (AllBlocks::CREATIVE_MOTOR != getBlock().mLegacyBlock) {
        return 0;
    }

    return convertToDirection(generatedSpeed->getValue(), getBlock().getState<FacingID>(VanillaStates::FacingDirection));
}