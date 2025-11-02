#pragma once
#include <memory>
#include <typeindex>
#include "flywheel/api/instance/Instancer.hpp"
#include "flywheel/api/instance/InstanceType.hpp"
#include "flywheel/api/model/Model.hpp"

class InstancerProviderBase {
public:
	virtual ~InstancerProviderBase() = default;

	virtual std::shared_ptr<InstancerBase> instancerBase(
		std::shared_ptr<void> typePtr, // type erased InstanceType<I>
		std::shared_ptr<Model> model,
		int bias,
		std::type_index typeInfo
	) = 0;
};	

class InstancerProvider : public InstancerProviderBase {
public:
	template <typename I>
	std::shared_ptr<Instancer<I>> instancer(
		std::shared_ptr<InstanceType<I>> type,
		std::shared_ptr<Model> model,
		int bias
	) {
		auto typePtr = std::static_pointer_cast<void>(type);
		auto base = instancerBase(typePtr, model, bias, typeid(I));
		return std::static_pointer_cast<Instancer<I>>(base);
	}
};