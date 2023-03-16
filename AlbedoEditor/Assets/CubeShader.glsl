/////////////////////////////////////// - VERTEX SHADER - ///////////////////////////////////////
#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ProjectionView;
uniform vec4 u_LightColor;
uniform vec4 u_Color;
uniform vec4 u_LightPos;
uniform mat4 u_Transform;
uniform int  u_TextureEnabled;

out vec2 v_TexCoord;
out vec4 v_Color;
flat out int v_TextureEnabled;

void main()
{
	v_TextureEnabled = u_TextureEnabled;
	v_Color = u_Color;
	v_TexCoord = a_TexCoord;
	gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

/////////////////////////////////////// - FRAGMENT SHADER - ///////////////////////////////////////

#type fragment
#version 430 core

layout(location = 0) out vec4 color;

in vec2 v_TexCoord;
in vec4 v_Color;
flat in int v_TextureEnabled;

uniform sampler2D u_Texture;

void main()
{
	if(v_TextureEnabled == 1)
	{
		color = texture(u_Texture, v_TexCoord) * v_Color;
	}
	else
	{
		color = v_Color;
	}
}