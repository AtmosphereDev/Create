#pragma once
#include "SmartBlockEntity.hpp"

template <typename T>
void SmartBlockEntity::addBehaviour(const BehaviourType& type, std::shared_ptr<T> behaviour) {
    behaviours[type] = behaviour;
}

template <typename T>
std::shared_ptr<T> SmartBlockEntity::getBehaviour(const BehaviourType& type) {
    auto it = behaviours.find(type);
    if (it == behaviours.end()) return nullptr;
    return std::dynamic_pointer_cast<T>(it->second);
}