#pragma once
class BlockActor;
class StructureTransform;

class TransformableBlockEntity {
public:
    virtual void transform(BlockActor& blockEntity, const StructureTransform& transform);
};