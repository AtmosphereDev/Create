#pragma once    
#include <memory>
#include "flywheel/api/instance/Instance.hpp"

class Instancer {
public:
    /**
	 * @return a handle to a new copy of this model.
	 */
    std::shared_ptr<Instance> createInstance();
};