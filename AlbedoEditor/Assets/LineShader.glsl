/////////////////////////////////////// - VERTEX SHADER - ///////////////////////////////////////
#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_ProjectionView;
uniform vec4 u_Color;
uniform mat4 u_Transform;

out vec4 v_Color;

void main()
{
	v_Color = u_Color;
	gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

/////////////////////////////////////// - FRAGMENT SHADER - ///////////////////////////////////////

#type fragment
#version 430 core

layout(location = 0) out vec4 color;

in vec4 v_Color;

void main()
{
	//color = texture(u_Texture, v_TexCoord) * vec4(1.0f, 1.0f, 1.0f, 1.0f);
	color = v_Color;
}