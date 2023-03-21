#type vertex
#version 330 core
layout (location = 0) in vec3 aPosition;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Transform;

void main()
{
    gl_Position = u_LightSpaceMatrix * u_Transform * vec4(aPosition, 1.0);
}

#type fragment
#version 330 core

void main()
{             
    // gl_FragDepth = gl_FragCoord.z;
}