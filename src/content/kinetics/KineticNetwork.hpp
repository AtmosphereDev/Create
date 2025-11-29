#pragma once
#include <amethyst/Imports.hpp>
#include <unordered_map>

class KineticBlockEntity;

class KineticNetwork {
public:
    uint64_t id;
    bool initialized;
    std::unordered_map<std::shared_ptr<KineticBlockEntity>, float> sources;
    std::unordered_map<std::shared_ptr<KineticBlockEntity>, float> members;

private:
    float currentCapacity;
    float currentStress;
    float unloadedCapacity;
    float unloadedStress;
    int unloadedMembers;

public:
    KineticNetwork() : id(0), initialized(false), currentCapacity(0), currentStress(0), unloadedCapacity(0), unloadedStress(0), unloadedMembers(0) {}

    void initFromTE(float maxStress, float currentStress, int membersCount);

    void addSilently(std::shared_ptr<KineticBlockEntity> be, float lastCapacity, float lastStress);

    void add(std::shared_ptr<KineticBlockEntity> be);

    void updateCapacityFor(std::shared_ptr<KineticBlockEntity> be, float capacity);
    
    void updateStressFor(std::shared_ptr<KineticBlockEntity> be, float stress);

    void remove(std::shared_ptr<KineticBlockEntity> be);

    void sync();

    void updateFromNetwork(std::shared_ptr<KineticBlockEntity> be);

    void updateCapacity();

    void updateStress();

    void updateNetwork();

    float calculateCapacity();

    float calculateStress();

    float getActualCapacityOf(std::shared_ptr<KineticBlockEntity> be);

    float getActualStressOf(std::shared_ptr<KineticBlockEntity> be);

    static float getStressMultiplierForSpeed(float speed);

    int getSize();
};