///////////////////////////////////////// - VERTEX SHADER - /////////////////////////////////////////
#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Normals;

uniform mat4 u_ProjectionView;
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

struct Material {
    sampler2D u_DiffuseMap;
    sampler2D u_SpecularMap;		
    float u_Shininess;
}; 

struct Light {
    vec3 u_Position;

    vec3 u_Ambient;
    vec3 u_Diffuse;
    vec3 u_Specular;
};

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

uniform vec3	 u_CameraPos;
uniform Light	 light;
uniform Material material;

void main()
{
	vec3 ambient = light.u_Ambient * texture(material.u_DiffuseMap, v_TexCoord).rgb;
	
	vec3 normal = normalize(v_Normal);
	vec3 lightDir = normalize(light.u_Position - v_FragPos);
	float diff = max(dot(normal, lightDir), 0.0);
	vec3 diffuse = light.u_Diffuse * diff * texture(material.u_DiffuseMap, v_TexCoord).rgb;
	
	vec3 viewDir = normalize(u_CameraPos - v_FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.u_Shininess);
    vec3 specular = light.u_Specular * spec * texture(material.u_SpecularMap, v_TexCoord).rgb;  
	
	vec3 result = ambient + diffuse + specular;
	color = vec4(result, 1.0);
}