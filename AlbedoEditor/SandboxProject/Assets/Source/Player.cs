using System;

using Albedo;

namespace Sandbox
{
    public class Player : Entity
    {
        private TransformComponent m_Transform;

        public float Speed = 1.0f;
        public float Time = 0.0f;

        void OnCreate()
        {
            Console.WriteLine($"Player.OnCreate - {ID}");

            m_Transform = GetComponent<TransformComponent>();
        }

        void OnUpdate(float ts)
        {
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
            Vector3 translation = m_Transform.Translation;
            translation += velocity * ts;
            m_Transform.Translation = translation;
        }

    }
}