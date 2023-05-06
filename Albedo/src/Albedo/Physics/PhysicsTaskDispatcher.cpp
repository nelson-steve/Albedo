#include "AlbedoPreCompiledHeader.h"

#include "PhysicsTaskDispatcher.h"
#include <PxPhysicsAPI.h>

namespace Albedo {
    void PhysicsTaskDispatcher::submitTask(physx::PxBaseTask& task) {
        // immediately place it on the task queue as background async
        physx::PxBaseTask* taskptr = &task;
        {
            taskptr->run();
            taskptr->release();
        };
    }

    uint32_t PhysicsTaskDispatcher::getWorkerCount() const {
        return static_cast<uint32_t>(1);
    }
}