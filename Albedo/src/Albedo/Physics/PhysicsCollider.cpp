#include "AlbedoPreCompiledHeader.h"

#include "PhysicsCollider.h"
#include "PhysicsSolver.h"
#include "Albedo/Renderer/Mesh.h"
#include "Conversions.h"

#include <PxPhysicsAPI.h>
#include <glm/gtc/quaternion.hpp>

namespace Albedo {

    using namespace physx;

    void PhysicsCollider::UpdateFilterData(RigidBodyDynamicComponent* body) {
        PxFilterData filterData;
        filterData.word0 = body->GetFilterGroup(); // word0 = own ID
        filterData.word1 = body->GetFilterMask();
        collider->setSimulationFilterData(filterData);
    }

    void PhysicsCollider::UpdateFilterData(RigidBodyStaticComponent* body) {
        PxFilterData filterData;
        filterData.word0 = body->GetFilterGroup(); // word0 = own ID
        filterData.word1 = body->GetFilterMask();
        collider->setSimulationFilterData(filterData);
    }

    void PhysicsCollider::SetType(CollisionType type)
    {
        switch (type) {
        case CollisionType::Collider:
            collider->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
            collider->setFlag(PxShapeFlag::eTRIGGER_SHAPE, false);
            break;
        case CollisionType::Trigger:
            collider->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
            collider->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
            break;
        }
    }

    PhysicsCollider::CollisionType PhysicsCollider::GetType() const
    {
        return collider->getFlags() & PxShapeFlag::eTRIGGER_SHAPE ? CollisionType::Trigger : CollisionType::Collider;
    }

    void PhysicsCollider::SetQueryable(bool state)
    {
        collider->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, state);
    }

    bool PhysicsCollider::GetQueryable() const
    {
        return collider->getFlags() & PxShapeFlag::eSCENE_QUERY_SHAPE;
    }

    void PhysicsCollider::SetRelativeTransform(const glm::vec3& position, const glm::quat& rotation)
    {
        collider->setLocalPose(glmToPhysx::ToTransform(position, rotation));
    }

    // --------------------------------------------- Box Collider ------------------------------------------------------------------//

    BoxCollider::BoxCollider(RigidBodyDynamicComponent* body, const glm::vec3& size, Ref<PhysicsMaterial> mat, const glm::vec3& position, const glm::quat& rotation)
    {
        collider = PxRigidActorExt::createExclusiveShape(*body->GetRigidActor(), PxBoxGeometry(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f), *(mat->GetPhysXmat()));
        if (!collider)
            Albedo_CORE_ASSERT(false, "collider initialization failed");
        collider->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        collider->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        SetRelativeTransform(position, rotation);
        //UpdateFilterData(body);
    }

    BoxCollider::BoxCollider(RigidBodyStaticComponent* body, const glm::vec3& size, Ref<PhysicsMaterial> mat, const glm::vec3& position, const glm::quat& rotation)
    {
        auto& s = size * 2.0f;
        collider = PxRigidActorExt::createExclusiveShape(*body->GetRigidActor(), PxBoxGeometry(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f), *(mat->GetPhysXmat()));
        if (!collider)
            Albedo_CORE_ASSERT(false, "collider initialization failed");
        collider->setFlag(PxShapeFlag::eSCENE_QUERY_SHAPE, true);
        collider->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
        SetRelativeTransform(position, rotation);
        //UpdateFilterData(body);
    }

    // --------------------------------------------- Sphere Collider ------------------------------------------------------------------//

    SphereCollider::SphereCollider(RigidBodyDynamicComponent* body, float r, Ref<PhysicsMaterial> mat, const glm::vec3& pos, const glm::quat& rot)
    {
        collider = PxRigidActorExt::createExclusiveShape(*body->GetRigidActor(), PxSphereGeometry(r), *mat->GetPhysXmat());
        if (!collider)
            Albedo_CORE_ASSERT(false, "collider initialization failed");
        SetRelativeTransform(pos, rot);
        //UpdateFilterData(body);
    }

    SphereCollider::SphereCollider(RigidBodyStaticComponent* body, float radius, Ref<PhysicsMaterial> mat, const glm::vec3& pos, const glm::quat& rot)
    {
        collider = PxRigidActorExt::createExclusiveShape(*body->GetRigidActor(), PxSphereGeometry(radius), *mat->GetPhysXmat());
        if (!collider)
            Albedo_CORE_ASSERT(false, "collider initialization failed");
        SetRelativeTransform(pos, rot);
        //UpdateFilterData(body);
    }
    
    void SphereCollider::SetRadius(float radius)
    {
        collider->getGeometry().sphere().radius = radius;
    }

    float SphereCollider::GetRadius() const 
    {
        return collider->getGeometry().sphere().radius;
    }

    // --------------------------------------------- Capsule Collider ------------------------------------------------------------------//

    CapsuleCollider::CapsuleCollider(RigidBodyDynamicComponent* body, float radius, float halfHeight, Ref<PhysicsMaterial> mat, const glm::vec3& pos, const glm::quat& rot)
    {
        collider = PxRigidActorExt::createExclusiveShape(*body->GetRigidActor(), PxCapsuleGeometry(radius, halfHeight), *mat->GetPhysXmat());

        SetRelativeTransform(pos, rot);
        //UpdateFilterData(body);
    }

    CapsuleCollider::CapsuleCollider(RigidBodyStaticComponent* body, float radius, float halfHeight, Ref<PhysicsMaterial> mat, const glm::vec3& pos, const glm::quat& rot)
    {
        collider = PxRigidActorExt::createExclusiveShape(*body->GetRigidActor(), PxCapsuleGeometry(radius, halfHeight), *mat->GetPhysXmat());

        SetRelativeTransform(pos, rot);
        //UpdateFilterData(body);
    }

    // --------------------------------------------- Mesh Collider ------------------------------------------------------------------//

    MeshCollider::MeshCollider(RigidBodyDynamicComponent* body, Ref<Mesh> meshAsset, Ref<PhysicsMaterial> mat)
    {
        auto& meshdata = meshAsset;

        std::vector<PxVec3> vertices(meshAsset->GetVertices().size());
        std::vector<PxU32> indices(meshdata->GetIndices().size());
        // only want positional data here, UVs and other data are not relevant
        for (int i = 0; i < vertices.size(); i++) 
        {
            vertices[i] = PxVec3(meshdata->GetVertices()[i].x, meshdata->GetVertices()[i].y, meshdata->GetVertices()[i].z);
        }

        for (int i = 0; i < indices.size(); i++) 
        {
            indices[i] = meshdata->GetIndices()[i];
        }

        // cooking data info
        PxTriangleMeshDesc meshDesc;
        meshDesc.setToDefault();
        meshDesc.points.data = &vertices[0];
        meshDesc.points.stride = sizeof(vertices[0]);
        assert(vertices.size() < std::numeric_limits<physx::PxU32>::max());
        meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());

        assert(indices.size() / 3 < std::numeric_limits<physx::PxU32>::max());
        meshDesc.triangles.count = static_cast<physx::PxU32>(indices.size() / 3);
        meshDesc.triangles.stride = 3 * sizeof(indices[0]);
        meshDesc.triangles.data = &indices[0];

        // specify width
        if (sizeof(indices[0]) == sizeof(uint16_t)) 
        {
            meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
        }    //otherwise assume 32 bit
        physx::PxTriangleMesh* triMesh = PhysicsSolver::cooking->createTriangleMesh(meshDesc, PhysicsSolver::phys->getPhysicsInsertionCallback());

        collider = PxRigidActorExt::createExclusiveShape(*body->GetRigidActor(), PxTriangleMeshGeometry(triMesh), *mat->GetPhysXmat());
        triMesh->release();
        //UpdateFilterData(body);
    }

    MeshCollider::MeshCollider(RigidBodyStaticComponent* body, Ref<Mesh> meshAsset, Ref<PhysicsMaterial> mat)
    {
        auto& meshdata = meshAsset;

        std::vector<PxVec3> vertices(meshAsset->GetVertices().size());
        std::vector<PxU32> indices(meshdata->GetIndices().size());
        // only want positional data here, UVs and other data are not relevant
        for (int i = 0; i < vertices.size(); i++)
        {
            vertices[i] = PxVec3(meshdata->GetVertices()[i].x, meshdata->GetVertices()[i].y, meshdata->GetVertices()[i].z);
        }

        for (int i = 0; i < indices.size(); i++)
        {
            indices[i] = meshdata->GetIndices()[i];
        }

        // cooking data info
        PxTriangleMeshDesc meshDesc;
        meshDesc.setToDefault();
        meshDesc.points.data = &vertices[0];
        meshDesc.points.stride = sizeof(vertices[0]);
        assert(vertices.size() < std::numeric_limits<physx::PxU32>::max());
        meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());

        assert(indices.size() / 3 < std::numeric_limits<physx::PxU32>::max());
        meshDesc.triangles.count = static_cast<physx::PxU32>(indices.size() / 3);
        meshDesc.triangles.stride = 3 * sizeof(indices[0]);
        meshDesc.triangles.data = &indices[0];

        // specify width
        if (sizeof(indices[0]) == sizeof(uint16_t))
        {
            meshDesc.flags = PxMeshFlag::e16_BIT_INDICES;
        }    //otherwise assume 32 bit
        physx::PxTriangleMesh* triMesh = PhysicsSolver::cooking->createTriangleMesh(meshDesc, PhysicsSolver::phys->getPhysicsInsertionCallback());

        collider = PxRigidActorExt::createExclusiveShape(*body->GetRigidActor(), PxTriangleMeshGeometry(triMesh), *mat->GetPhysXmat());
        triMesh->release();
        //UpdateFilterData(body);
    }

    // --------------------------------------------- Convex Mesh Collider ------------------------------------------------------------------//

    ConvexMeshCollider::ConvexMeshCollider(RigidBodyDynamicComponent* body, Ref<Mesh> meshAsset, Ref<PhysicsMaterial> mat)
    {
        auto& meshdata = meshAsset;

        // only want positional data here, UVs and other data are not relevant
        std::vector<PxVec3> vertices(meshdata->GetVertices().size());
        for (int i = 0; i < vertices.size(); i++) 
        {
            vertices[i] = PxVec3(meshdata->GetVertices()[i].x, meshdata->GetVertices()[i].y, meshdata->GetVertices()[i].z);
        }

        PxBoundedData pointdata;
        assert(vertices.size() < std::numeric_limits<physx::PxU32>::max());
        pointdata.count = static_cast<physx::PxU32>(vertices.size());
        pointdata.stride = sizeof(PxVec3);
        pointdata.data = &vertices[0];

        PxConvexMeshDesc meshDesc;
        meshDesc.setToDefault();
        meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
        meshDesc.points = pointdata;
        meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

        physx::PxConvexMesh* convMesh = PhysicsSolver::cooking->createConvexMesh(meshDesc, PhysicsSolver::phys->getPhysicsInsertionCallback());
        
        collider = PxRigidActorExt::createExclusiveShape(*body->GetRigidActor(), PxConvexMeshGeometry(convMesh), *mat->GetPhysXmat());
        //UpdateFilterData(body);
    }

    ConvexMeshCollider::ConvexMeshCollider(RigidBodyStaticComponent* body, Ref<Mesh> meshAsset, Ref<PhysicsMaterial> mat) 
    {
        auto& meshdata = meshAsset;

        // only want positional data here, UVs and other data are not relevant
        std::vector<PxVec3> vertices(meshdata->GetVertices().size());
        for (int i = 0; i < vertices.size(); i++)
        {
            vertices[i] = PxVec3(meshdata->GetVertices()[i].x, meshdata->GetVertices()[i].y, meshdata->GetVertices()[i].z);
        }

        PxBoundedData pointdata;
        assert(vertices.size() < std::numeric_limits<physx::PxU32>::max());
        pointdata.count = static_cast<physx::PxU32>(vertices.size());
        pointdata.stride = sizeof(PxVec3);
        pointdata.data = &vertices[0];

        PxConvexMeshDesc meshDesc;
        meshDesc.setToDefault();
        meshDesc.points.count = static_cast<physx::PxU32>(vertices.size());
        meshDesc.points = pointdata;
        meshDesc.flags = PxConvexFlag::eCOMPUTE_CONVEX;

        physx::PxConvexMesh* convMesh = PhysicsSolver::cooking->createConvexMesh(meshDesc, PhysicsSolver::phys->getPhysicsInsertionCallback());

        collider = PxRigidActorExt::createExclusiveShape(*body->GetRigidActor(), PxConvexMeshGeometry(convMesh), *mat->GetPhysXmat());
        //UpdateFilterData(body);
    }

    //Transformation PhysicsCollider::GetRelativeTransform() const {
    //    auto pose = collider->getLocalPose();
    //    return Transformation{ glm::vec3(pose.p.x,pose.p.y,pose.p.z),glm::quat(pose.q.w, pose.q.x,pose.q.y,pose.q.z) };
    //}
    //
    //matrix4 PhysicsCollider::CalculateWorldMatrix(const Transform& tr) const {
    //    return tr.CalculateWorldMatrix() * (matrix4)GetRelativeTransform();
    //}
    //
    //void BoxCollider::DebugDraw(DebugDrawer& dbg, color_t debug_color, const Transform& tr) const {
    //    dbg.DrawRectangularPrism(CalculateWorldMatrix(tr), debug_color, glm::vec3(extent.x * 2, extent.y * 2, extent.z * 2));
    //}
    //
    //void SphereCollider::DebugDraw(DebugDrawer& dbg, color_t debug_color, const Transform& tr) const {
    //    dbg.DrawSphere(CalculateWorldMatrix(tr), debug_color, GetRadius());
    //}
    //
    //void CapsuleCollider::DebugDraw(DebugDrawer& dbg, color_t debug_color, const Transform& tr) const {
    //    dbg.DrawCapsule(glm::translate(glm::rotate(CalculateWorldMatrix(tr), deg_to_rad(90), glm::vec3(0, 0, 1)), glm::vec3(0, -halfHeight, 0)), debug_color, radius, halfHeight * 2);
    //}
}