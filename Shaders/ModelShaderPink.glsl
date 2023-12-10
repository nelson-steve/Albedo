#type vertex
#version 330 core
layout (location = 0) in vec3 a_Position;

uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;

void main()
{
    gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
layout (location = 0) out vec4 Color;

void main()
{
    vec3 pinkColor = vec3(0.97, 0.78, 0.86);    
    Color = vec4(pinkColor, 1.0);
}
