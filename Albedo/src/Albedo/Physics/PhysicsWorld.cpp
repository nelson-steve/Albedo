#include "AlbedoPreCompiledHeader.h"

#include "PhysicsWorld.h"

namespace Albedo {

	void PhysicsWorld::Update(const Timestep& ts, PhysicsComponent& phyCmp, TransformComponent& transformComp)
	{
		phyCmp.Force += phyCmp.Mass * m_Gravity;
		phyCmp.Velocity += phyCmp.Force / phyCmp.Mass * ts.GetTime();
		phyCmp.Position += phyCmp.Velocity * ts.GetTime();
		phyCmp.Force = glm::vec3(0);
		
		transformComp.Position = phyCmp.Position;
	}

}