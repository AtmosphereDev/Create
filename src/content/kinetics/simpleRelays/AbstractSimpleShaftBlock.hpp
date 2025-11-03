#pragma once
#include "content/kinetics/simpleRelays/AbstractShaftBlock.hpp"

class AbstractSimpleShaftBlock : public AbstractShaftBlock {
public:
    AbstractSimpleShaftBlock(const std::string& name, short id, const Material& material)
        : AbstractShaftBlock(name, id, material) {}
};