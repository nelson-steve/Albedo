using System;

using Albedo;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;
        private Physics2DComponent m_RigidBody;

        public float Speed = 1.0f;
        public float Time = 0.0f;

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();
            m_RigidBody = GetComponent<Physics2DComponent>();

            m_RigidBody.EnableGravity(false);
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

            Time += ts;
            // Console.WriteLine($"Player.OnUpdate: {ts}");

            float speed = Speed;
            Vector3 velocity = Vector3.Zero;
            
            if (Input.IsKeyDown(KeyCode.W))
            {
                velocity.Y = 1.0f;
                Console.WriteLine("W key pressed");

            }
            if (Input.IsKeyDown(KeyCode.S))
            {
                velocity.Y = -1.0f;
                Console.WriteLine("S key pressed");
            }
            if (Input.IsKeyDown(KeyCode.A))
            {
                velocity.X = -1.0f;
                Console.WriteLine("A key pressed");
            }
            if (Input.IsKeyDown(KeyCode.D))
            {
                velocity.X = 1.0f;
                Console.WriteLine("D key pressed");
            }
            if (Input.IsKeyDown(KeyCode.Q))
            {
                velocity.Z = -1.0f;
                Console.WriteLine("Q key pressed");
            }
            if (Input.IsKeyDown(KeyCode.E))
            {
                velocity.Z = 1.0f;
                Console.WriteLine("E key pressed");
            }

            //Entity cameraEntity = FindEntityByName("Camera");
            //if (cameraEntity != null)
            //{
            //    Camera camera = cameraEntity.As<Camera>();
            //
            //    if (Input.IsKeyDown(KeyCode.Q))
            //        camera.DistanceFromPlayer += speed * 2.0f * ts;
            //    else if (Input.IsKeyDown(KeyCode.E))
            //        camera.DistanceFromPlayer -= speed * 2.0f * ts;
            //}

            velocity *= speed * ts;

            m_RigidBody.ApplyLinearImpulse(velocity.XY, true);

            //Vector3 translation = m_Transform.Translation;
            //translation += velocity * ts;
            //m_Transform.Translation = translation;
        }

    }
}