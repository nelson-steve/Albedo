#pragma once

#include <task/PxCpuDispatcher.h>

namespace Albedo {
    class PhysicsTaskDispatcher : public physx::PxCpuDispatcher 
    {
    public:
        // invoked by PhysX to enqueue a new task
        void submitTask(physx::PxBaseTask& task) final;
        uint32_t getWorkerCount() const final;

        virtual ~PhysicsTaskDispatcher() {}

    };
}
