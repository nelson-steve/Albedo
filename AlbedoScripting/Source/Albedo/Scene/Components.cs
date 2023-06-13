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