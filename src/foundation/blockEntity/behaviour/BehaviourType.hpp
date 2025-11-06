#pragma once
#include <amethyst/Imports.hpp>
#include <functional>

//template<typename T>
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

    bool operator==(const BehaviourType& other) const noexcept {
        return name.getHash() == other.name.getHash();
    }
};

namespace std {
    template <>
    struct hash<BehaviourType> {
        std::size_t operator()(const BehaviourType& type) const noexcept {
            return type.hashCode();
        }
    };
}