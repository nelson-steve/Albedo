#include "AlbedoPreCompiledHeader.h"

#include "PhysicsWorld.h"

#include <PxPhysicsAPI.h>

#define PVD_HOST "127.0.0.1"
#define STATIC(a) decltype(a) a

STATIC(Albedo::PhysicsWorld::phys);
STATIC(Albedo::PhysicsWorld::pvd);
STATIC(Albedo::PhysicsWorld::cooking);
STATIC(Albedo::PhysicsWorld::scene);
STATIC(Albedo::PhysicsWorld::gDefaultErrorCallback);
STATIC(Albedo::PhysicsWorld::gDefaultAllocatorCallback);
STATIC(Albedo::PhysicsWorld::foundation);

namespace Albedo {

    physx::PxFilterFlags _FilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
    {
        // let triggers through
        if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
        {
            pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
            return physx::PxFilterFlag::eDEFAULT;
        }
        // generate contacts for all that were not filtered above
        pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

        // trigger the contact callback for pairs (A,B) where
        // the filtermask of A contains the ID of B and vice versa.
        if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
            pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS | physx::PxPairFlag::eNOTIFY_TOUCH_LOST | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;

        return physx::PxFilterFlag::eDEFAULT;
    }

	PhysicsWorld::PhysicsWorld()
	{
        if (foundation == nullptr)
        {
            foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
        }
        if (foundation == nullptr)
        {
            Albedo_CORE_ASSERT(false, "PhysX foundation failed to create");
        }
        bool recordMemoryAllocations = true;
        if (pvd == nullptr)
        {
            pvd = physx::PxCreatePvd(*foundation);
            physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
            pvd->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
            if (phys == nullptr)
            {
                phys = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation, physx::PxTolerancesScale(), recordMemoryAllocations, pvd);
            }
            if (phys == nullptr)
            {
                Albedo_CORE_ASSERT(false, "PhysX Physics initialization failed");
            }
        }

        //create PhysX scene
        physx::PxSceneDesc desc(phys->getTolerancesScale());
        desc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);

        //set the dispatcher
        desc.cpuDispatcher = &taskDispatcher;

        desc.filterShader = &_FilterShader;
        desc.simulationEventCallback = this;

        // initialize cooking library with defaults
        if (foundation)
            cooking = PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physx::PxCookingParams(physx::PxTolerancesScale()));
        if (!cooking)
        {
            Albedo_CORE_ASSERT(false, "PhysX Cooking initialization failed");
        }

        // setup cooking parameters (used for all subsequent calls to cooking)
        physx::PxTolerancesScale scale;
        physx::PxCookingParams params(scale);
        // disable mesh cleaning - perform mesh validation on development configurations
        //params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
        // disable edge precompute, edges are set for each triangle, slows contact generation
        //params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

        cooking->setParams(params);

        // initialize extensions (can be omitted, these are optional components)
        if (!PxInitExtensions(*phys, pvd))
        {
            Albedo_CORE_ASSERT(false, "Unable to initialize PhysX");
        }

        //create the scene
        scene = phys->createScene(desc);
        if (!scene)
        {
            Albedo_CORE_ASSERT(false, "PhysX Scene failed to create");
        }
	}

	PhysicsWorld::~PhysicsWorld()
	{

	}

	void PhysicsWorld::Init()
	{
        m_ColliderConfig = std::make_shared<ColliderConfig>();
        m_RigidBodyConfig = std::make_shared<RigidBodyConfig>();
        
        {
            mat = phys->createMaterial(m_ColliderConfig->width, m_ColliderConfig->height, 0);
            rigidActor = phys->createRigidDynamic(physx::PxTransform(physx::PxVec3(0, 0, 0)));
            collider = physx::PxRigidActorExt::createExclusiveShape(*rigidActor, physx::PxBoxGeometry(m_ColliderConfig->width, m_ColliderConfig->height, m_ColliderConfig->depth), *(mat));
            rigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
            rigidActor->setMass(.0f);
            physx::PxQuat rot(1.0, physx::PxVec3(0, 0, 0));
            physx::PxTransform transform(physx::PxVec3(0, 0, 0), rot);
            rigidActor->setGlobalPose(transform);
        }

        {
            mat2 = phys->createMaterial(m_ColliderConfig->width, m_ColliderConfig->height, -1);
            rigidActor2 = phys->createRigidDynamic(physx::PxTransform(physx::PxVec3(0, 100, 0)));
            collider2 = physx::PxRigidActorExt::createExclusiveShape(*rigidActor2, physx::PxBoxGeometry(m_ColliderConfig->width, m_ColliderConfig->height, m_ColliderConfig->depth), *(mat2));
            rigidActor2->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
            rigidActor2->setMass(0.1f);
            physx::PxQuat rot2(physx::PxHalfPi, physx::PxVec3(0, 0, 1));
            physx::PxTransform transform2(physx::PxVec3(0, 100, 0), rot2);
            rigidActor2->setGlobalPose(transform2);
        }
        
        //rigidActor->setMassSpaceInertiaTensor(physx::PxVec3(1.0f));
        //rigidActor->setAngularVelocity(physx::PxVec3(2.0f, 0.0f, 0.0f), true);
        
        scene->addActor(*rigidActor);
        scene->addActor(*rigidActor2);


        // create simulation
        //mat = phys->createMaterial(0.5f, 0.5f, 0.6f);
        //physx::PxRigidStatic* groundPlane = PxCreatePlane(*phys, physx::PxPlane(0, 1, 0, 50), *mat);
        //scene->addActor(*groundPlane);
        //
        //float halfExtent = .5f;
        //physx::PxShape* shape = phys->createShape(physx::PxBoxGeometry(halfExtent, halfExtent, halfExtent), *mat);
        //physx::PxU32 size = 30;
        //physx::PxTransform t(physx::PxVec3(0));
        //for (physx::PxU32 i = 0; i < size; i++) {
        //    for (physx::PxU32 j = 0; j < size - i; j++) {
        //        physx::PxTransform localTm(physx::PxVec3(physx::PxReal(j * 2) - physx::PxReal(size - i), physx::PxReal(i * 2 + 1), 0) * halfExtent);
        //        rigidActor = phys->createRigidDynamic(t.transform(localTm));
        //        rigidActor->attachShape(*shape);
        //        physx::PxRigidBodyExt::updateMassAndInertia(*rigidActor, 10.0f);
        //        scene->addActor(*rigidActor);
        //    }
        //}
        //shape->release();
	}

	void PhysicsWorld::Update(Timestep ts)
	{
        scene->lockWrite();
        //for (int i = 0; i < nsteps; i++)
        {
            scene->simulate(ts.GetTime());
            scene->fetchResults(true);      //simulate is async, this blocks until the results have been calculated
        }
        scene->unlockWrite();
        auto pos = rigidActor->getGlobalPose();
        auto pos2 = rigidActor2->getGlobalPose();
        //auto pos = rigidActor->getGlobalPose();
        
        Albedo_Core_INFO("pos : {0} {1} {2}", pos.p.x, pos.p.y, pos.p.z);
        Albedo_Core_INFO("pos2: {0} {1} {2}", pos2.p.x, pos2.p.y, pos2.p.z);

	}
}