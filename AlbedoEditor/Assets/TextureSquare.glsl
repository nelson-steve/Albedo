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

struct DirLight {
    vec3 u_Direction;
	
    vec3 u_Ambient;
    vec3 u_Diffuse;
    vec3 u_Specular;
};

struct PointLight {
    vec3 u_Position;
    
    float u_Constant;
    float u_Linear;
    float u_Quadratic;
	
    vec3 u_Ambient;
    vec3 u_Diffuse;
    vec3 u_Specular;
};

struct SpotLight {
    vec3  u_Position;
    vec3  u_Direction;
    float u_CutOff;
    float u_OuterCutOff;
  
    float u_Constant;
    float u_Linear;
    float u_Quadratic;
  
    vec3 u_Ambient;
    vec3 u_Diffuse;
    vec3 u_Specular;       
};

#define NR_POINT_LIGHTS 4

in vec2 v_TexCoord;
in vec3 v_FragPos;
in vec3 v_Normal;

uniform vec3	   u_CameraPos;
uniform DirLight   dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight  spotLight;
uniform Material   material;

// function prototypes
//vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
//vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
//vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
	//vec3 normal = normalize(v_Normal);
	//vec3 viewDir = normalize(u_CameraPos - v_FragPos);

    //vec3 result = CalcDirLight(dirLight, norm, viewDir);

    //for(int i = 0; i < NR_POINT_LIGHTS; i++)
        //result += CalcPointLight(u_PointLights[i], norm, v_FragPos, viewDir);

    //result += CalcSpotLight(spotLight, norm, v_FragPos, viewDir);
    
    //FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    //vec3 lightDir = normalize(-light.u_Direction);
    //// diffuse shading
    //float diff = max(dot(normal, lightDir), 0.0);
    //// specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.u_Shininess);
    //// combine results
    //vec3 ambient  = light.u_Ambient  * vec3(texture(material.u_Diffuse, v_TexCoords));
    //vec3 diffuse  = light.u_Diffuse  * diff * vec3(texture(material.u_Diffuse,  v_TexCoords));
    //vec3 specular = light.u_Specular * spec * vec3(texture(material.u_Specular, v_TexCoords));
    //return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    //vec3 lightDir = normalize(light.u_Position - fragPos);
    //// diffuse shading
    //float diff = max(dot(normal, lightDir), 0.0);
    //// specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.u_Shininess);
    //// attenuation
    //float distance = length(light.u_Position - v_FragPos);
    //float attenuation = 1.0 / (light.u_Constant + light.u_Linear * distance + light.u_Quadratic * (distance * distance));    
    //// combine results
    //vec3 ambient  = light.u_Ambient  * vec3(texture(material.u_Diffuse, v_TexCoords));
    //vec3 diffuse  = light.u_Diffuse  * diff * vec3(texture(material.u_Diffuse, v_TexCoords));
    //vec3 specular = light.u_Specular * spec * vec3(texture(material.u_Specular, v_TexCoords));
    //ambient *= attenuation;
    //diffuse *= attenuation;
    //specular *= attenuation;
    //return (ambient + diffuse + specular);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    //vec3 lightDir = normalize(light.u_Position - v_FragPos);
    //// diffuse shading
    //float diff = max(dot(normal, lightDir), 0.0);
    //// specular shading
    //vec3 reflectDir = reflect(-lightDir, normal);
    //float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.u_Shininess);
    //// attenuation
    //float distance = length(light.u_Position - fragPos);
    //float attenuation = 1.0 / (light.u_Constant + light.u_Linear * distance + light.u_Quadratic * (distance * distance));    
    //// spotlight intensity
    //float theta = dot(lightDir, normalize(-light.direction)); 
    //float epsilon = light.u_CutOff - light.u_OuterCutOff;
    //float intensity = clamp((theta - light.u_OuterCutOff) / epsilon, 0.0, 1.0);
    //// combine results
    //vec3 ambient = light.ambient * vec3(texture(material.u_Diffuse, v_TexCoords));
    //vec3 diffuse = light.diffuse * diff * vec3(texture(material.u_Diffuse, v_TexCoords));
    //vec3 specular = light.specular * spec * vec3(texture(material.u_Specular, v_TexCoords));
    //ambient *= attenuation * intensity;
    //diffuse *= attenuation * intensity;
    //specular *= attenuation * intensity;
    //return (ambient + diffuse + specular);
}