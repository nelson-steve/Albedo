/////////////////////////////////////// - VERTEX SHADER - ///////////////////////////////////////
#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;

out vec2 TexCoords;

out VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;

void main()
{
    vs_out.FragPos = vec3(u_Transform * vec4(a_Position, 1.0));
    vs_out.Normal = mat3(transpose(inverse(u_Transform))) * a_Normal;
    vs_out.TexCoords = a_UV;
    gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}

/////////////////////////////////////// - FRAGMENT SHADER - ///////////////////////////////////////

#type fragment
#version 430 core
out vec4 FragColor;

in VS_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
} fs_in;

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

#define TOTAL_POINT_LIGHTS 128

uniform sampler2D u_DiffuseMap;

uniform DirLight   u_DirLight;
uniform PointLight u_PointLights[TOTAL_POINT_LIGHTS];
uniform SpotLight  u_SpotLight;
uniform int u_NoOfPointLights;
uniform float u_Shininess;
uniform vec3 u_Specular;
uniform int u_SpotLightExists;
uniform int u_PointLightExists;
uniform int u_DirLightExists;

uniform vec3 u_LightPosition;
uniform vec3 u_CameraPosition;
uniform vec3 u_MaterialColor;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.u_Direction);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);
    
    vec3 ambient  = light.u_Ambient  * vec3(texture(u_DiffuseMap, fs_in.TexCoords));
    vec3 diffuse  = light.u_Diffuse  * diff * vec3(texture(u_DiffuseMap,  fs_in.TexCoords));
    vec3 specular = light.u_Specular * spec;

    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.u_Position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);

    float distance = length(light.u_Position - fragPos);
    float attenuation = 1.0 / (light.u_Constant + light.u_Linear * distance + light.u_Quadratic * (distance * distance));    

    vec3 ambient  = light.u_Ambient  * vec3(texture(u_DiffuseMap, fs_in.TexCoords));
    vec3 diffuse  = light.u_Diffuse  * diff * vec3(texture(u_DiffuseMap, fs_in.TexCoords));
    vec3 specular = light.u_Specular * spec;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.u_Position - fragPos);

    float diff = max(dot(normal, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), u_Shininess);

    float distance = length(light.u_Position - fragPos);
    float attenuation = 1.0 / (light.u_Constant + light.u_Linear * distance + light.u_Quadratic * (distance * distance));    

    float theta = dot(lightDir, normalize(-light.u_Direction)); 
    float epsilon = light.u_CutOff - light.u_OuterCutOff;
    float intensity = clamp((theta - light.u_OuterCutOff) / epsilon, 0.0, 1.0);

    vec3 ambient  = light.u_Ambient * vec3(texture(u_DiffuseMap, fs_in.TexCoords));
    vec3 diffuse  = light.u_Diffuse * diff * vec3(texture(u_DiffuseMap, fs_in.TexCoords));
    vec3 specular = light.u_Specular * spec;
    ambient  *= attenuation * intensity;
    diffuse  *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 color = texture(u_DiffuseMap, fs_in.TexCoords).rgb;
    //vec3 normal = normalize(fs_in.Normal);
    vec3 normal = normalize(fs_in.Normal);
    vec3 lightColor = vec3(0.3);
    // ambient
    vec3 ambient = 0.3 * lightColor;
    // diffuse
    vec3 lightDir = normalize(u_LightPosition - fs_in.FragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * lightColor;
    // specular
    vec3 viewDir = normalize(u_CameraPosition - fs_in.FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = 0.0;
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    spec = pow(max(dot(normal, halfwayDir), 0.0), 64.0);
    vec3 specular = spec * lightColor;    
    //vec3 lighting = (1.0 - shadow) * color;
    vec3 lighting = vec3(0.1) * color;
    lighting = CalcDirLight(u_DirLight, normal, viewDir);
    //if(u_DirLightExists == 1)
    //{
    //}
    //vec3 lighting;
    for(int i = 0; i < u_NoOfPointLights; i++)
    {
        lighting += CalcPointLight(u_PointLights[i], normal, fs_in.FragPos, viewDir);
    }
    lighting += CalcSpotLight(u_SpotLight, normal, fs_in.FragPos, viewDir);
    //lighting *= color;
    //lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
    //
    FragColor = vec4(lighting, 1.0);
}