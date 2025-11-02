#pragma once
#include <memory>
#include "flywheel/api/instance/InstancerProvider.hpp"
#include "content/kinetics/base/RotatingInstance.hpp"
#include "flywheel/api/model/Model.hpp"
#include "flywheel/api/instance/InstanceHandle.hpp"

class TestHandle : public InstanceHandle {
public:
	virtual void setChanged() override  {
		Log::Info("TestHandle::setChanged called");
	}

    virtual void setDeleted() override  {
		Log::Info("TestHandle::setDeleted called");
	}

    virtual void setVisible(bool visible) override  {
		Log::Info("TestHandle::setVisible called with {}", visible);
	}

    virtual bool isVisible() const override {
		Log::Info("TestHandle::isVisible called");
		return true;
	}
};

template <typename I>
class TestInstancer : public Instancer<I> {
public:
	std::shared_ptr<Model> model;
	std::shared_ptr<InstanceType<I>> typePtr;

	TestInstancer(std::shared_ptr<InstanceType<I>> typePtr, std::shared_ptr<Model> model, int bias)
		: Instancer<I>(), model(model), typePtr(typePtr) {}

	virtual std::shared_ptr<I> createInstance() override {
		auto handle = std::make_shared<TestHandle>();
		Log::Info("TestInstancer::createInstance called!");
		return std::make_shared<I>(typePtr, handle);
	}
};

class InstancerProviderImpl : public InstancerProvider {
public:
	std::unordered_map<std::type_index, std::shared_ptr<InstancerBase>> instancers;

    virtual std::shared_ptr<InstancerBase> instancerBase(
		std::shared_ptr<void> typePtr, // type erased InstanceType<I>
		std::shared_ptr<Model> model,
		int bias,
		std::type_index typeInfo
	) override {
        auto it = instancers.find(typeInfo);
        if (it != instancers.end()) {
            return it->second;
        }

		if (typeInfo == typeid(RotatingInstance)) {
			auto typedPtr = std::static_pointer_cast<InstanceType<RotatingInstance>>(typePtr);
			auto instancer = std::make_shared<TestInstancer<RotatingInstance>>(typedPtr, model, bias);
			instancers[typeInfo] = instancer;
			return instancer;
		}

		AssertFail("No instancer not implemented for type {}", typeInfo.name());
    }
};