using System;
using System.Dynamic;

namespace Albedo
{
    public abstract class Component
    {
        public Entity Entity { get; internal set; }
    }

    public class TransformComponent : Component
    {
        public Vector3 Translation
        {
            get
            {
                InternalCalls.TransformComponent_GetTranslation(Entity.ID, out Vector3 translation);
                return translation;
            }
            set
            {
                InternalCalls.TransformComponent_SetTranslation(Entity.ID, ref value);
            }
        }
    }

    public class ScriptComponent : Component
    { 
        
    }

    public class MeshComponent : Component
    {

    }

    public class BoxCollider2DComponent : Component
    { 
    
    }

    public class Physics2DComponent : Component
    {
        public void EnableGravity(bool gravity)
        {
            InternalCalls.Rigidbody2DComponent_EnableGravity(Entity.ID, gravity);
        }

        public void ApplyLinearImpulse(Vector2 impulse, Vector2 worldPosition, bool wake)
        {
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulse(Entity.ID, ref impulse, ref worldPosition, wake);
        }

        public void ApplyLinearImpulse(Vector2 impulse, bool wake)
        {
            Console.WriteLine($"Linear impulse {impulse.X}, {impulse.Y}");
            InternalCalls.Rigidbody2DComponent_ApplyLinearImpulseToCenter(Entity.ID, ref impulse, wake);
        }
    }

    public class PhysicsComponent : Component
    {

    }

    public class TagComponent : Component
    {

    }

    public class ShaderComponent : Component
    {

    }

    public class ColliderComponent : Component
    {

    }

    public class TextureComponent : Component
    {

    }

    public class MaterialComponent : Component
    {

    }

    public class LightComponent : Component
    {

    }

    public class SkyboxComponent : Component
    {

    }

}