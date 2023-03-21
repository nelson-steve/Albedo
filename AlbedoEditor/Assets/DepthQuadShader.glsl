#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    gl_Position = vec4(a_Position, 1.0);
}
#type fragment
#version 330 core
out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D depthMap;
uniform float u_Near_plane;
uniform float u_Far_plane;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // Back to NDC 
    return (2.0 * u_Near_plane * u_Far_plane) / (u_Far_plane + u_Near_plane - z * (u_Far_plane - u_Near_plane));	
}

void main()
{             
    float depthValue = texture(depthMap, v_TexCoord).r;
    // FragColor = vec4(vec3(LinearizeDepth(depthValue) / u_Far_plane), 1.0); // perspective
    color = vec4(vec3(depthValue), 1.0); // orthographic
}