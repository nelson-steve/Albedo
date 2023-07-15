using System;

using Albedo;

namespace Sandbox
{
    public class Enemy : Entity
    {
        private TransformComponent m_Transform;

        public float Speed = 1.0f;
        public float Time = 0.0f;
        public bool alive = true;

        void OnCreate()
        {
            m_Transform = GetComponent<TransformComponent>();
        }

        void OnUpdate(float ts)
        {
            Time += ts;

            float speed = Speed;
            Vector3 velocity = Vector3.Zero;
            velocity.X = 1.0f;

            velocity *= speed * ts;
            Vector3 translation = m_Transform.Translation;
            translation += velocity * ts;
            m_Transform.Translation = translation;
        }
    }
}
