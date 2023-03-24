#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;

uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
out vec4 FragColor;

in vec2 v_TexCoord;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_AOMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_RoughnessMap;


void main()
{    
    //FragColor = texture(albedo, v_TexCoord);
    FragColor = mix(texture(albedo, v_TexCoord), texture(ao, v_TexCoord), 1.0);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
