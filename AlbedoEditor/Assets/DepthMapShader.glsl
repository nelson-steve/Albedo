/////////////////////////////////////// - VERTEX SHADER - ///////////////////////////////////////
#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_TexCoord;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Transform;

void main()
{
    gl_Position = u_LightSpaceMatrix * u_Transform * vec4(a_Position, 1.0);
}

/////////////////////////////////////// - FRAGMENT SHADER - ///////////////////////////////////////

#type fragment
#version 430 core

void main()
{             
    gl_FragDepth = gl_FragCoord.z;
}