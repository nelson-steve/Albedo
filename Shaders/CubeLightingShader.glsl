///////////////////////////////////////// - VERTEX SHADER - /////////////////////////////////////////
#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat4 u_Transform;
uniform int  u_TextureEnabled;

out vec2 v_TexCoord;
out vec3 v_FragPos;
out vec3 v_Normal;
flat out int v_TextureEnabled;

void main()
{
	v_FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
	v_TextureEnabled = u_TextureEnabled;
	v_Normal = mat3(transpose(inverse(u_Transform))) * a_Normals;
	v_TexCoord = a_TexCoord;
	gl_Position = u_ProjectionView * vec4(v_FragPos, 1.0);
}

///////////////////////////////////////// - FRAGMENT SHADER - /////////////////////////////////////////

#type fragment
#version 430 core

out vec4 color;

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal; 
flat in int v_TextureEnabled;

uniform vec3 u_LightPos;
uniform vec3 u_CameraPos;
uniform vec3 u_LightColor;
uniform vec3 u_MaterialColor;
uniform sampler2D u_Texture;

void main()
{
	float ambientStrength = 0.3;
	vec3 ambient = ambientStrength * u_LightColor;

	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(u_LightPos - v_FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = diff * u_LightColor;

	float specularStrength = 0.5;
    vec3 viewDir = normalize(u_CameraPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * u_LightColor;

	vec3 result = (ambient + diffuse + specular) * u_MaterialColor;

	if(v_TextureEnabled == 1)
	{
		color = texture(u_Texture, v_TexCoord) * vec4(result, 1.0);
	}
	else
	{
		color = vec4(result, 1.0);
	}
}