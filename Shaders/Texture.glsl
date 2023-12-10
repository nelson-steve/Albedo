#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV0;
layout(location = 3) in vec2 a_UV1;

uniform mat4 u_ProjectionView;
uniform mat4 u_Model;
uniform mat3 u_NormalMatrix;

out vec2 v_UV0;
out vec2 v_UV1;
out vec3 v_WorldPos;
out vec3 v_Normal;

void main()
{
    v_UV0 = a_UV0;
    v_UV1 = a_UV1;
    v_WorldPos = vec3(u_Model * vec4(a_Position, 1.0));
    v_Normal = u_NormalMatrix * a_Normal; 
    gl_Position = u_ProjectionView * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
layout (location = 0) out vec4 Color;

in vec2 v_UV0;
in vec2 v_UV1;
in vec3 v_WorldPos;
in vec3 v_Normal;

uniform vec3 u_CamPos;

const float PI = 3.14159265359;

uniform sampler2D u_Albedo;
uniform sampler2D u_MetallicRoughness;
uniform sampler2D u_Normal;
uniform sampler2D u_Occlusion;
uniform sampler2D u_Emissive;

uniform int u_AlbedoExists;
uniform int u_MetallicRoughnessExists;
uniform int u_NormalExists;
uniform int u_OcclusionExists;
uniform int u_EmissiveExists;

uniform int u_AlbedoSet;
uniform int u_MetallicRoughnessSet;
uniform int u_NormalSet;
uniform int u_OcclusionSet;
uniform int u_EmissiveSet;

vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_Normal, v_UV0).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(v_WorldPos);
    vec3 Q2  = dFdy(v_WorldPos);
    vec2 st1 = dFdx(v_UV0);
    vec2 st2 = dFdy(v_UV0);

    vec3 N   = normalize(v_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
void main()
{
    vec3 albedo     = pow(texture(u_Albedo, v_UV0).rgb, vec3(2.2));
    float metallic  = texture(u_MetallicRoughness, v_UV0).r;
    float roughness = texture(u_MetallicRoughness, v_UV0).r;
    float ao        = texture(u_Occlusion, v_UV0).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(u_CamPos - v_WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    
    vec3 lightPos = vec3(0.0f, 1.0f, 0.0f);
    vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);
    for(int i = 0; i < 1; ++i) 
    {
        // calculate per-light radiance
        vec3 L = normalize(lightPos - v_WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(lightPos - v_WorldPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = lightColor * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);   
        float G   = GeometrySmith(N, V, L, roughness);      
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);
           
        vec3 numerator    = NDF * G * F; 
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
        vec3 specular = numerator / denominator;
        
        // kS is equal to Fresnel
        vec3 kS = F;
        // for energy conservation, the diffuse and specular light can't
        // be above 1.0 (unless the surface emits light); to preserve this
        // relationship the diffuse component (kD) should equal 1.0 - kS.
        vec3 kD = vec3(1.0) - kS;
        // multiply kD by the inverse metalness such that only non-metals 
        // have diffuse lighting, or a linear blend if partly metal (pure metals
        // have no diffuse light).
        kD *= 1.0 - metallic;	  

        // scale light by NdotL
        float NdotL = max(dot(N, L), 0.0);        

        // add to outgoing radiance Lo
        Lo += (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
    }
    // ambient lighting (note that the next IBL tutorial will replace 
    // this ambient lighting with environment lighting).
    vec3 ambient = vec3(0.2) * albedo * ao;
    
    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    Color = vec4(color, 1.0);
    //Color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
}
