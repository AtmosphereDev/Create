#pragma once
#include <amethyst/Imports.hpp>

template<typename T>
class BehaviourType {
private:
    HashedString name;

public:
    BehaviourType(const HashedString& name) : name(name) {}

    const HashedString& getName() const {
        return name;
    }

    std::size_t hashCode() const {
        return name.getHash();
    }
};