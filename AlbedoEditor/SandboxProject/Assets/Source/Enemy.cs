using System;

using Albedo;

namespace Sandbox
{
    public class Enemy : Entity
    {
        private TransformComponent m_Transform;
        private Physics2DComponent m_RigidBody;

        private Vector3 oldPosition;

        void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
            m_RigidBody = GetComponent<Physics2DComponent>();

            m_RigidBody.EnableGravity(false);

            oldPosition = m_Transform.Translation;
        }

        void OnUpdate(float ts)
        {
            if (m_Transform == null)
            {
                Console.WriteLine("TransformComponent not found");
                return;
            }
            if (m_RigidBody == null)
            {
                Console.WriteLine("Physics2DComponent not found");
                return;
            }

            Vector3 currentPosition = m_Transform.Translation;
            if(currentPosition.Equals(oldPosition))
            {
                oldPosition = currentPosition;
            }
            else
            {
                m_RigidBody.EnableGravity(true);
            }

        }
    }
}
