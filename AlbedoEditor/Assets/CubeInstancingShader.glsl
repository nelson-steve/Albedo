/////////////////////////////////////// - VERTEX SHADER - ///////////////////////////////////////
#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 aOffset;

uniform mat4 u_ProjectionView;
uniform vec4 u_Color;
uniform mat4 u_Transform;

out vec4 v_Color;

uniform vec3 offsets[100];

void main()
{
	v_Color = u_Color;
	vec3 offset = offsets[gl_InstanceID];
	gl_Position = u_ProjectionView * vec4(a_Position + offset, 1.0);
}

/////////////////////////////////////// - FRAGMENT SHADER - ///////////////////////////////////////

#type fragment
#version 430 core

layout(location = 0) out vec4 color;

in vec4 v_Color;

uniform sampler2D u_Texture;

void main()
{
	int v_TilingFactor = 1;

	//color *= texture(u_Texture, v_TexCoord * v_TilingFactor);
	color = v_Color;
}