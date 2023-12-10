#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV0;

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

out vec2 v_UV0;
out vec3 v_Normal;
out vec3 v_WorldPos;

void main()
{
    v_WorldPos = a_Position;
    v_UV0 = a_UV0;
    v_Normal = u_NormalMatrix * a_Normal;
    gl_Position = u_ProjectionView * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
layout (location = 0) out vec4 Color;

uniform vec3 u_CamPos;

in vec2 v_UV0;
in vec3 v_Normal;
in vec3 v_WorldPos;

void main()
{
    Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
