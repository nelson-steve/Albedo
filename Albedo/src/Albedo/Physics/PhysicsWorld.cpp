#include "AlbedoPreCompiledHeader.h"

#include "PhysicsWorld.h"
#include "SphererCollider.h"

#include <VHACD.h>

namespace Albedo {

    static inline bool tryAxis(
        const ColliderComponent& one,
        const ColliderComponent& two,
        const glm::vec3& axis,
        const glm::vec3& toCentre,
        uint32_t index,

        // These values may be updated
        float& smallestPenetration,
        uint32_t& smallestCase
    )
    {
        // Make sure we have a normalized axis, and don't check almost parallel axes
        if (axis.squareMagnitude() < 0.0001) return true;
        axis.normalise();

        real penetration = penetrationOnAxis(one, two, axis, toCentre);

        if (penetration < 0) return false;
        if (penetration < smallestPenetration) {
            smallestPenetration = penetration;
            smallestCase = index;
        }
        return true;
    }

    void RigidBody::CalculateDerivedData()
    {

    }

    void RigidBody::Integrate()
    {

    }


	void PhysicsWorld::Update(const Timestep& ts, PhysicsComponent& phyCmp, TransformComponent& transformComp, ColliderComponent& colliderComp)
	{
		//phyCmp.Force += phyCmp.Mass * m_Gravity;
		//phyCmp.Velocity += phyCmp.Force / phyCmp.Mass * ts.GetTime();
		//transformComp.Position += phyCmp.Velocity * ts.GetTime();
		//phyCmp.Force = glm::vec3(0);
		//std::dynamic_pointer_cast<SphereCollider>(colliderComp.collider)->SetCenter(transformComp.Position);
		
		Update();
	}

	void PhysicsWorld::Update()
	{
		CheckIntersections();
	}

	void PhysicsWorld::CheckIntersections()
	{
		for (auto& collider1 : m_Colliders)
		{
			for (auto& collider2 : m_Colliders)
			{
				if (collider2 == collider1) continue;
				switch (collider1.collider->GetType())
				{
					case Type::Sphere:
					{
						const auto& c1 = std::dynamic_pointer_cast<SphereCollider>(collider1.collider);
						const auto& c2 = std::dynamic_pointer_cast<SphereCollider>(collider2.collider);
						auto a = c1->GetCenter();
						auto b = c2->GetRadius();
						float distance = glm::dot((c1->GetCenter() - c1->GetCenter()), (c1->GetCenter() - c1->GetCenter()));
						if (distance < (c2->GetRadius() + c2->GetRadius()) * (c2->GetRadius() + c2->GetRadius()))
							Albedo_Core_INFO("Collision");
						break;
					}
					case Type::BoxAABB:
					{
						const auto& c1 = std::dynamic_pointer_cast<BoxCollider>(collider1.collider);
						const auto& c2 = std::dynamic_pointer_cast<BoxCollider>(collider2.collider);
						if (c1->GetMin().x > c2->GetMax().x || c1->GetMax().x < c2->GetMin().x)
							break;
						else if (c1->GetMin().y > c2->GetMax().y || c1->GetMax().y < c2->GetMin().y)
							break;
						else if (c1->GetMin().z > c2->GetMax().z || c1->GetMax().z < c2->GetMin().z)
							break;
						else
						{
							m_IntersectingColliders.push_back(std::pair<ColliderComponent, ColliderComponent>(collider1, collider2));
							Albedo_Core_INFO("Collision");
							break;
						}
					}
					default:
						Albedo_Core_ERROR("Error: unkown collider type");
						break;
				}
			}
		}

#if 0
			auto v = VHACD::CreateVHACD();
			VHACD::IVHACD::Parameters p;

			//load obj and save count
			//e.g. uint32_t tcount = w.loadObj(inputFile);

			p.m_maxConvexHulls = 32;

			p.m_minimumVolumePercentErrorAllowed = false;

			p.m_maxRecursionDepth = 20;


			p.m_shrinkWrap = false;
			if (p.m_shrinkWrap)
			{
				if (p.m_shrinkWrap)
				{
					printf("Shrinkwrap enabled.\n");
				}
				else
				{
					printf("Shrinkwrap disabled.\n");
				}
			}

			p.m_fillMode = VHACD::FillMode::FLOOD_FILL;

			p.m_maxNumVerticesPerCH = 23;

			p.m_asyncACD = false;

			p.m_findBestPlane = false;

			VHACD::IVHACD* iface = VHACD::CreateVHACD();

			//double* points = new double[w.mVertexCount * 3];
			//for (uint32_t i = 0; i < w.mVertexCount * 3; i++)
			//{
			//	points[i] = w.mVertices[i];
			//}
			//bool canceled = false;
			//
			//iface->Compute(points, w.mVertexCount, w.mIndices, w.mTriCount, p);
#endif
	}

	void PhysicsWorld::CheckCollisions()
	{
		for (auto& i : m_IntersectingColliders)
		{
            //if (!IntersectionTests::boxAndBox(one, two)) return 0;

            // Find the vector between the two centres
            glm::mat4 t;
            glm::vec3 s = t[3];
            glm::vec3 toCentre = i.first.rigidBody->GetAxis(3) - i.second.rigidBody->GetAxis(3);
            
            #define DBL_MAX          1.7976931348623158e+308 // max value
            // We start assuming there is no contact
            float pen = DBL_MAX;
            uint32_t best = 0xffffff;

            // Now we check each axes, returning if it gives us
            // a separating axis, and keeping track of the axis with
            // the smallest penetration otherwise.
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(0), toCentre, 0, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(1), toCentre, 1, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(2), toCentre, 2, pen, best)) return;

            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(0), toCentre, 3, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(1), toCentre, 4, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(2), toCentre, 5, pen, best)) return;

            // Store the best axis-major, in case we run into almost
            // parallel edge collisions later
            unsigned bestSingleAxis = best;

            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(0) % i.second.rigidBody->GetAxis(0), toCentre, 6, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(0) % i.second.rigidBody->GetAxis(1), toCentre, 7, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(0) % i.second.rigidBody->GetAxis(2), toCentre, 8, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(1) % i.second.rigidBody->GetAxis(0), toCentre, 9, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(1) % i.second.rigidBody->GetAxis(1), toCentre, 10, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(1) % i.second.rigidBody->GetAxis(2), toCentre, 11, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(2) % i.second.rigidBody->GetAxis(0), toCentre, 12, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(2) % i.second.rigidBody->GetAxis(1), toCentre, 13, pen, best)) return;
            if (!tryAxis(i.first, i.second, i.first.rigidBody->GetAxis(2) % i.second.rigidBody->GetAxis(2), toCentre, 14, pen, best)) return;

            // Make sure we've got a result.
            Albedo_CORE_ASSERT(best != 0xffffff, "error");

            // We now know there's a collision, and we know which
            // of the axes gave the smallest penetration. We now
            // can deal with it in different ways depending on
            // the case.
            if (best < 3)
            {
                // We've got a vertex of box two on a face of box one.
                fillPointFaceBoxBox(one, two, toCentre, data, best, pen);
                data->addContacts(1);
                return 1;
            }
            else if (best < 6)
            {
                // We've got a vertex of box one on a face of box two.
                // We use the same algorithm as above, but swap around
                // one and two (and therefore also the vector between their
                // centres).
                fillPointFaceBoxBox(two, one, toCentre * -1.0f, data, best - 3, pen);
                data->addContacts(1);
                return 1;
            }
            else
            {
                // We've got an edge-edge contact. Find out which axes
                best -= 6;
                unsigned oneAxisIndex = best / 3;
                unsigned twoAxisIndex = best % 3;
                Vector3 oneAxis = one.getAxis(oneAxisIndex);
                Vector3 twoAxis = two.getAxis(twoAxisIndex);
                Vector3 axis = oneAxis % twoAxis;
                axis.normalise();

                // The axis should point from box one to box two.
                if (axis * toCentre > 0) axis = axis * -1.0f;

                // We have the axes, but not the edges: each axis has 4 edges parallel
                // to it, we need to find which of the 4 for each object. We do
                // that by finding the point in the centre of the edge. We know
                // its component in the direction of the box's collision axis is zero
                // (its a mid-point) and we determine which of the extremes in each
                // of the other axes is closest.
                Vector3 ptOnOneEdge = one.halfSize;
                Vector3 ptOnTwoEdge = two.halfSize;
                for (unsigned i = 0; i < 3; i++)
                {
                    if (i == oneAxisIndex) ptOnOneEdge[i] = 0;
                    else if (one.getAxis(i) * axis > 0) ptOnOneEdge[i] = -ptOnOneEdge[i];

                    if (i == twoAxisIndex) ptOnTwoEdge[i] = 0;
                    else if (two.getAxis(i) * axis < 0) ptOnTwoEdge[i] = -ptOnTwoEdge[i];
                }

                // Move them into world coordinates (they are already oriented
                // correctly, since they have been derived from the axes).
                ptOnOneEdge = one.transform * ptOnOneEdge;
                ptOnTwoEdge = two.transform * ptOnTwoEdge;

                // So we have a point and a direction for the colliding edges.
                // We need to find out point of closest approach of the two
                // line-segments.
                Vector3 vertex = contactPoint(
                    ptOnOneEdge, oneAxis, one.halfSize[oneAxisIndex],
                    ptOnTwoEdge, twoAxis, two.halfSize[twoAxisIndex],
                    bestSingleAxis > 2
                );

                // We can fill the contact.
                Contact* contact = data->contacts;

                contact->penetration = pen;
                contact->contactNormal = axis;
                contact->contactPoint = vertex;
                contact->setBodyData(one.body, two.body,
                    data->friction, data->restitution);
                data->addContacts(1);
                return 1;
            }
            return 0;
		}
	}
}