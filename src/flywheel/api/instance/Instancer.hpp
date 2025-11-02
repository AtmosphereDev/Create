#pragma once    
#include <memory>
#include "flywheel/api/instance/Instance.hpp"

class InstancerBase {
public:
    virtual ~InstancerBase() = default;
};

template <typename I>
class Instancer : public InstancerBase {
public:
    /**
	 * @return a handle to a new copy of this model.
	 */
    virtual std::shared_ptr<I> createInstance() = 0;

    // default void createInstances(I[] arr) {
	// 	for (int i = 0; i < arr.length; i++) {
	// 		arr[i] = createInstance();
	// 	}
	// }

    /**
	 * Steal an instance from another instancer.
	 * <br>
	 * This has the effect of swapping the instance's model in-place.
	 * <br><br>
	 * If the given instance is already owned by this instancer, this method does nothing.
	 * <br>
	 * If the given instance is {@code null}, this method does nothing.
	 * <br>
	 * If the given instance was created by a different backend, the behavior of this method is undefined.
	 *
	 * @param instance The instance to steal.
	 */
	// void stealInstance(@Nullable I instance);
};