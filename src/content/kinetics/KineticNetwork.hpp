#pragma once
#include <amethyst/Imports.hpp>
#include <unordered_map>
#include "src/content/kinetics/base/KineticBlockEntity.hpp"

class KineticNetwork {
public:
    uint64_t id;
    bool initialized;
    std::unordered_map<KineticBlockEntity*, float> sources;
    std::unordered_map<KineticBlockEntity*, float> members;

private:
    float currentCapacity;
    float currentStress;
    float unloadedCapacity;
    float unloadedStress;
    int unloadedMembers;

public:
    KineticNetwork() : id(0), initialized(false), currentCapacity(0), currentStress(0), unloadedCapacity(0), unloadedStress(0), unloadedMembers(0) {}

    void initFromTE(float maxStress, float currentStress, int membersCount) {
        unloadedCapacity = maxStress;
        unloadedStress = currentStress;
        unloadedMembers = membersCount;
        initialized = true;
        updateStress();
        updateCapacity();
    }

    void addSilently(KineticBlockEntity* be, float lastCapacity, float lastStress) {
        if (members.find(be) != members.end())
            return;

        if (be->isSource()) {
            unloadedCapacity -= lastCapacity * getStressMultiplierForSpeed(be->getGeneratedSpeed());
            float addedStressCapacity = be->calculateAddedStressCapacity();
            sources[be] = addedStressCapacity;
        }

        unloadedStress -= lastStress * getStressMultiplierForSpeed(be->getTheoreticalSpeed());
        float stressApplied = be->calculateStressApplied();
        members[be] = stressApplied;

        unloadedMembers--;
        if (unloadedMembers < 0) unloadedMembers = 0;
        if (unloadedCapacity < 0) unloadedCapacity = 0;
        if (unloadedStress < 0) unloadedStress = 0;
    }

    void add(KineticBlockEntity* be) {
        if (members.find(be) != members.end())
            return;
        if (be->isSource())
            sources[be] = be->calculateAddedStressCapacity();
        members[be] = be->calculateStressApplied();
        updateFromNetwork(be);
        be->networkDirty = true;
    }

    void updateCapacityFor(KineticBlockEntity* be, float capacity) {
        sources[be] = capacity;
        updateCapacity();
    }

    void updateStressFor(KineticBlockEntity* be, float stress) {
        members[be] = stress;
        updateStress();
    }

    void remove(KineticBlockEntity* be) {
        if (members.find(be) == members.end())
            return;
        if (be->isSource())
            sources.erase(be);
        members.erase(be);
        be->updateFromNetwork(0, 0, 0);

        if (members.empty()) {
            TorquePropagator::networks[be->level->mId].erase(this->id);
            return;
        }

        auto it = members.begin();
        if (it != members.end()) {
            it->first->networkDirty = true;
        }
    }

    void sync() {
        for (auto& [be, _] : members)
            updateFromNetwork(be);
    }

    void updateFromNetwork(KineticBlockEntity* be) {
        be->updateFromNetwork(currentCapacity, currentStress, getSize());
    }

    void updateCapacity() {
        float newMaxStress = calculateCapacity();
        if (currentCapacity != newMaxStress) {
            currentCapacity = newMaxStress;
            sync();
        }
    }

    void updateStress() {
        float newStress = calculateStress();
        if (currentStress != newStress) {
            currentStress = newStress;
            sync();
        }
    }

    void updateNetwork() {
        float newStress = calculateStress();
        float newMaxStress = calculateCapacity();
        if (currentStress != newStress || currentCapacity != newMaxStress) {
            currentStress = newStress;
            currentCapacity = newMaxStress;
            sync();
        }
    }

    float calculateCapacity() {
        float presentCapacity = 0;
        for (auto it = sources.begin(); it != sources.end(); ) {
            KineticBlockEntity* be = it->first;
            if (be->level->mBlockSource->getBlockEntity(be->getBlockPos()) != be) {
                it = sources.erase(it);
                continue;
            }
            presentCapacity += getActualCapacityOf(be);
            ++it;
        }
        return presentCapacity + unloadedCapacity;
    }

    float calculateStress() {
        float presentStress = 0;
        for (auto it = members.begin(); it != members.end(); ) {
            KineticBlockEntity* be = it->first;
            if (be->level->mBlockSource->getBlockEntity(be->getBlockPos()) != be) {
                it = members.erase(it);
                continue;
            }
            presentStress += getActualStressOf(be);
            ++it;
        }
        return presentStress + unloadedStress;
    }

    float getActualCapacityOf(KineticBlockEntity* be) {
        return sources[be] * getStressMultiplierForSpeed(be->getGeneratedSpeed());
    }

    float getActualStressOf(KineticBlockEntity* be) {
        return members[be] * getStressMultiplierForSpeed(be->getTheoreticalSpeed());
    }

    static float getStressMultiplierForSpeed(float speed) {
        return std::abs(speed);
    }

    int getSize() {
        return unloadedMembers + static_cast<int>(members.size());
    }
};