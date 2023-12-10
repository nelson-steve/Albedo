#type vertex
#version 330 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec3 a_Normal;
layout (location = 2) in vec2 a_UV;

uniform mat4 u_ProjectionView;

out vec2 v_TexCoord;

void main()
{
	v_TexCoord = a_UV;
	gl_Position = u_ProjectionView * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;

uniform sampler2D u_Texture;

void main()
{
	//color = texture(u_Texture, v_TexCoord);
	color = texture(u_Texture, v_TexCoord);
}

