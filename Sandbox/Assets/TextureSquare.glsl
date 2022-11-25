#type vertex
#version 330 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec4 a_Color;

uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;
uniform vec4 u_Color;

out vec4 v_Color;
out vec4 v_Position;

void main()
{
	v_Position = vec4(a_Position, 1.0f);
	v_Color = u_Color;
	gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 Color;

uniform vec4 u_Color;

in vec4 v_Color;
in vec4 v_Position;

void main()
{
	Color = v_Color;
}