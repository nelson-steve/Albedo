#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;

uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;

out vec2 v_TexCoord;
out vec3 v_Color;

void main()
{
    v_TexCoord = a_UV;
    v_Color = vec3(a_Position/2);
    gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord;
in vec3 v_Color;

uniform sampler2D u_AlbedoMap;
//uniform sampler2D u_DiffuseMap;
//uniform sampler2D u_AOMap;
//uniform sampler2D u_MetallicMap;
//uniform sampler2D u_NormalMap;
//uniform sampler2D u_RoughnessMap;


void main()
{    
    
    //vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
    FragColor = texture(u_AlbedoMap, v_TexCoord);
    //FragColor = mix(texture(albedo, v_TexCoord), texture(ao, v_TexCoord), 1.0);
    //FragColor = vec4(v_Color, 1.0);
}
