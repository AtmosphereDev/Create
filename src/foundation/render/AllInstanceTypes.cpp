#include "AllInstanceTypes.hpp"
#include "flywheel/lib/instance/SimpleInstanceType.hpp"

std::shared_ptr<InstanceType<RotatingInstance>> AllInstanceTypes::ROTATING =
    SimpleInstanceType<RotatingInstance>::builder(
        [](std::shared_ptr<InstanceType<RotatingInstance>> type,
           std::shared_ptr<InstanceHandle> handle) {
            return std::make_shared<RotatingInstance>(
                std::static_pointer_cast<InstanceTypeBase>(type),
                handle
            );
        }
    ).build();