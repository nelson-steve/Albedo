#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;
layout (location = 3) in int a_ID;

uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec3 v_Color;
flat out int v_ID;

void main()
{
    v_ID = a_ID;
    v_TexCoord = a_UV;
    v_Color = vec3(a_Position/2);
    gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
layout (location = 0) out vec4 Color;
layout (location = 1) out int ID;

in vec2 v_TexCoord;
in vec3 v_Color;
flat in int v_ID;


uniform int u_DefaultTexture;

uniform sampler2D u_Default;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_AOMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_RoughnessMap;


void main()
{    
    if(u_DefaultTexture == 1)
    {
        Color = texture(u_Default, v_TexCoord);
    }
    else
    {
        Color = texture(u_AlbedoMap, v_TexCoord);
    }
    ID = v_ID;
    //Color = mix(texture(u_AlbedoMap, v_TexCoord), texture(u_AOMap, v_TexCoord), 0.2);
}
