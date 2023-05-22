#include "AlbedoPreCompiledHeader.h"

#include "PhysicsSolver.h"
//#include "PhysicsBodyComponent.hpp"
//#include "App.hpp"
//#include "PhysXDefines.h"
//#include "Entity.hpp"
//#include <snippetcommon/SnippetPVD.h>
//#include <extensions/PxDefaultSimulationFilterShader.h>
#define PX_RELEASE(x)    if(x)    { x->release(); x = NULL;    }

#include <thread>

#define PVD_HOST "127.0.0.1"

#define STATIC(a) decltype(a) a

STATIC(Albedo::PhysicsSolver::phys);
STATIC(Albedo::PhysicsSolver::pvd);
STATIC(Albedo::PhysicsSolver::cooking);
STATIC(Albedo::PhysicsSolver::scene);
STATIC(Albedo::PhysicsSolver::gDefaultErrorCallback);
STATIC(Albedo::PhysicsSolver::gDefaultAllocatorCallback);
STATIC(Albedo::PhysicsSolver::foundation);

#undef STATIC(a) decltype(a) a

namespace Albedo {

    physx::PxFilterFlags FilterShader(physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0, physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1, physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize)
    {
        PX_UNUSED(attributes0);
        PX_UNUSED(attributes1);
        PX_UNUSED(filterData0);
        PX_UNUSED(filterData1);
        PX_UNUSED(constantBlockSize);
        PX_UNUSED(constantBlock);

        // let triggers through
        if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
        {
            pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
            return physx::PxFilterFlag::eDEFAULT;
        }

        pairFlags = physx::PxPairFlag::eSOLVE_CONTACT
            | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT
            | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
            | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS
            | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS
            | physx::PxPairFlag::eDETECT_CCD_CONTACT;

        // generate contacts for all that were not filtered above
        //pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

        // trigger the contact callback for pairs (A,B) where
        // the filtermask of A contains the ID of B and vice versa.
        //if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
        //    pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS | physx::PxPairFlag::eNOTIFY_TOUCH_LOST | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;

        return physx::PxFilterFlag::eDEFAULT;
    }

    void PhysicsSolver::Init()
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
            pvd = PxCreatePvd(*foundation);
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

        desc.filterShader = FilterShader;
        desc.simulationEventCallback = this;
        desc.solverType = physx::PxSolverType::ePGS;
        //desc.flags |= physx::PxSceneFlag::eREQUIRE_RW_LOCK;

#if ENABLE_GPU
        PxCudaContextManagerDesc cudaContextManagerDesc;
        gCudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc, PxGetProfilerCallback());
        sceneDesc.cudaContextManager = gCudaContextManager;
        sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;
#endif

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

        PhysicsSolver::cooking->setParams(params);

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

        physx::PxMaterial* terrainMaterial;
        terrainMaterial = phys->createMaterial(0.4f, 0.4f, .5f);

        //physx::PxRigidStatic* groundPlane = physx::PxCreatePlane(*phys, physx::PxPlane(0, 1, 0, 0), *terrainMaterial);
        //scene->addActor(*groundPlane);
        scene->setGravity(physx::PxVec3{0.0, -1.0, 0.0});
    }

    PhysicsSolver::~PhysicsSolver()
    {
        Deallocate();
    }

    void PhysicsSolver::AddActor(const RigidBodyDynamicComponent& rg)
    {
        scene->addActor(*rg.GetRigidActor());
    }

    void PhysicsSolver::UpdatePhysics(Timestep ts)
    {

        //auto step = scaleFactor / (GetApp()->evalNormal / 2);

        //physics substepping
        //int nsteps = ceil(step / max_step_time);
        //float step_time = step / nsteps;
        scene->lockWrite();
        //for (int i = 0; i < nsteps; i++)
        {
            scene->simulate(ts.GetTime());
            scene->fetchResults(true);      //simulate is async, this blocks until the results have been calculated
        }
        scene->unlockWrite();
    }

    void PhysicsSolver::Deallocate() {
        if (scene != nullptr) 
        {
            PX_RELEASE(scene);
        }
    }

    void PhysicsSolver::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
    {
        Albedo_Core_INFO("contact found");
        for (physx::PxU32 i = 0; i < nbPairs; ++i) {
            const physx::PxContactPair& contactpair = pairs[i];

            //if these actors do not exist in the scene anymore due to deallocation, do not process
            if (pairHeader.actors[0]->userData == nullptr || pairHeader.actors[1]->userData == nullptr) {
                continue;
            }

            Entity actor1_e, actor2_e;

            //get the physics body component stored in the user data (non-owning pointer!)
            std::memcpy(&actor1_e, &pairHeader.actors[0]->userData, sizeof(actor1_e));
            std::memcpy(&actor2_e, &pairHeader.actors[1]->userData, sizeof(actor2_e));

            //auto& actor1 = actor1_e.GetAllComponentsPolymorphic<PhysicsBodyComponent>()[0];
            //auto& actor2 = actor2_e.GetAllComponentsPolymorphic<PhysicsBodyComponent>()[0];

            size_t numContacts = 0;
            //stackarray(contactPoints, ContactPairPoint, contactpair.contactCount);
            {
                // do we need contact data?
                //if (actor1.GetWantsContactData() || actor2.GetWantsContactData()) 
                {
                    //stackarray(points, physx::PxContactPairPoint, contactpair.contactCount);
                    //auto count = contactpair.extractContacts(points, contactpair.contactCount);
                    //for (int i = 0; i < contactpair.contactCount; i++) {
                    //    contactPoints[i] = points[i];
                    //}
                    //numContacts = count;
                }
            }

            //invoke events
            if (contactpair.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {

                //actor1.OnColliderEnter(actor2, contactPoints, numContacts);
                //actor2.OnColliderEnter(actor1, contactPoints, numContacts);
            }

            if (contactpair.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
                //actor1.OnColliderExit(actor2, contactPoints, numContacts);
                //actor2.OnColliderExit(actor1, contactPoints, numContacts);
            }

            if (contactpair.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS) {
                //actor1.OnColliderPersist(actor2, contactPoints, numContacts);
                //actor2.OnColliderPersist(actor1, contactPoints, numContacts);
            }

        }
    }

    void PhysicsSolver::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
    {
        Albedo_Core_INFO("trigger found");
        for (physx::PxU32 i = 0; i < count; ++i) {
            // ignore pairs when shapes have been deleted
            const physx::PxTriggerPair& cp = pairs[i];
            if (cp.flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER)) {
                continue;
            }

            //if these actors do not exist in the scene anymore due to deallocation, do not process
            if (cp.otherActor->userData == nullptr || cp.triggerActor->userData == nullptr) {
                continue;
            }

            Entity other_e;
            Entity trigger_e;

            memcpy(&other_e, &cp.otherActor->userData, sizeof(other_e));
            memcpy(&trigger_e, &cp.triggerActor->userData, sizeof(trigger_e));

            //auto& other = other_e.GetAllComponentsPolymorphic<PhysicsBodyComponent>()[0];
            //auto& trigger = trigger_e.GetAllComponentsPolymorphic<PhysicsBodyComponent>()[0];

            //process events
            if (cp.status & (physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)) {
                //other.OnTriggerEnter(trigger);
                //trigger.OnTriggerEnter(other);
            }

            if (cp.status & (physx::PxPairFlag::eNOTIFY_TOUCH_LOST)) {
                //other.OnTriggerExit(trigger);
                //trigger.OnTriggerExit(other);
            }
        }
    }

    // Invoked by PhysX after simulation each tick
#if 0
    void PhysicsSolver::DeallocatePhysx() {
        if (scene != nullptr) {
            PX_RELEASE(scene);
        }
    }

    void PhysicsSolver::ReleaseStatics() {
        PX_RELEASE(phys);
        PX_RELEASE(foundation);
        PX_RELEASE(cooking);
    }

    bool PhysicsSolver::Raycast(const vector3& origin, const vector3& direction, decimalType maxDistance, RaycastHit& out_hit)
    {
        PxRaycastBuffer hit;
        bool result = scene->raycast(PxVec3(origin.x, origin.y, origin.z), PxVec3(direction.x, direction.y, direction.z), maxDistance, hit);

        //construct hit result
        out_hit = RaycastHit(hit);
        return result;
    }

    bool PhysicsSolver::BoxOverlap(const vector3& origin, const quaternion& r, const vector3& half_ext, OverlapHit& out_hit)
    {
        return generic_overlap(PhysicsTransform(origin, r), PxBoxGeometry(half_ext.x, half_ext.y, half_ext.z), out_hit);
    }

    bool PhysicsSolver::SphereOverlap(const vector3& origin, decimalType radius, OverlapHit& out_hit)
    {
        return generic_overlap(PhysicsTransform(origin, quaternion()), PxSphereGeometry(radius), out_hit);
    }

    bool PhysicsSolver::CapsuleOverlap(const vector3& origin, const quaternion& rotation, decimalType radius, decimalType halfheight, OverlapHit& out_hit)
    {
        return generic_overlap(PhysicsTransform(origin, rotation), PxCapsuleGeometry(radius, halfheight), out_hit);
    }

    bool PhysicsSolver::generic_overlap(const PhysicsTransform& t, const PxGeometry& geo, OverlapHit& out_hit)
    {
        PxOverlapBuffer hit;
        bool result = scene->overlap(geo, PxTransform(PxVec3(t.pos.x, t.pos.y, t.pos.y), PxQuat(t.rot.x, t.rot.y, t.rot.z, t.rot.w)), hit);
        out_hit = OverlapHit(hit);
        return result;
    }


    /**
     Make the physics system aware of an object
     @param e the entity to add
     */
    void PhysicsSolver::Spawn(PhysicsBodyComponent& actor) {
        auto e = actor.GetOwner();
        if (actor.rigidActor->userData == nullptr) {
            memcpy(&actor.rigidActor->userData, &e, sizeof(e)); // store the ID inside the userdata variable (NOT a pointer)
        }
        scene->lockWrite();
        scene->addActor(*(actor.rigidActor));
        scene->unlockWrite();
    }

    /**
     Remove the entity from the physics system. This does NOT destroy it in the World.
     @param e the entity to remove
     */
    void PhysicsSolver::Destroy(PhysicsBodyComponent& body) {
        scene->lockWrite();
        scene->removeActor(*(body.rigidActor));
        scene->unlockWrite();
    }

    /**
     Run the appropriate number of physics time steps given a frame rate scale
     @param deltaTime the scale factor to apply
     */

    //constructor which configures PhysX

    PhysicsSolver::RaycastHit::RaycastHit(const physx::PxRaycastBuffer& hit) :
        hasBlocking(hit.hasBlock),
        hitPosition(vector3(hit.block.position.x, hit.block.position.y, hit.block.position.z)),
        hitNormal(vector3(hit.block.normal.x, hit.block.normal.y, hit.block.normal.z)),
        hitDistance(hit.block.distance) {

        if (hit.hasBlock) {
            hitObject = (entity_t(uintptr_t(hit.block.actor->userData)));
        }
    }

    Entity PhysicsSolver::RaycastHit::getEntity() const
    {
        return Entity(hitObject);
    }
#endif
}