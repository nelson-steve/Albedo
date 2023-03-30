#type vertex
#version 430 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV;
//layout(location = 3) in mat4 a_Offset;

//precision highp float;
uniform mat4 u_ProjectionView;
uniform mat4 u_Transform;

//uniform mat4 offsets[100];

//uniform mat4 modelViewMatrix;
//uniform mat3 normalMatrix;
//uniform mat4 textureRotation;
//uniform vec2 textureRepeat;

out vec2 v_TexCoords;
out vec3 v_WorldPos;
out vec3 v_Normal;

void main()
{
    //mat4 offset = offsets[gl_InstanceID];
    v_WorldPos  = vec3(u_Transform * vec4(a_Position, 1.0));
    v_Normal    = normalize(a_Normal);
    //v_TexCoords = mat2(textureRotation)*(uv*textureRepeat);  
    v_TexCoords = a_UV;  
    gl_Position = u_ProjectionView * u_Transform * vec4(a_Position, 1.0);
}


#type fragment
#version 430 core

//precision highp float;
// Based on https://learnopengl.com/PBR/Theory
// textures from https://freepbr.com/
layout (location = 0) out vec4 FragColor;

in vec2 v_TexCoords;
in vec3 v_WorldPos;
in vec3 v_Normal;

// material parameters
uniform sampler2D u_AlbedoMap;
uniform sampler2D u_AOMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_RoughnessMap;

uniform float     u_RoughnessScale;
// lights
//uniform vec3 lightPositions[4];
uniform vec3  u_LightPosition;
uniform vec3  u_LightColor;
uniform float u_Exposure;
uniform vec3  u_CameraPosition;

const float PI = 3.14159265359;
// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_NormalMap, v_TexCoords).xyz * 2.0 - 1.0;
    
    vec3 Q1  = dFdx(v_WorldPos);
    vec3 Q2  = dFdy(v_WorldPos);
    vec2 st1 = dFdx(v_TexCoords);
    vec2 st2 = dFdy(v_TexCoords);
    
    vec3 N   = normalize(v_Normal);
    vec3 T   = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B   = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);
    
    return normalize(TBN * tangentNormal);
    //return vec3(0.0, 0.0, 0.0);
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
    //return 0.0;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
    
    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;
    
    return nom / denom;
    //return 0.0;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
    
    return ggx1 * ggx2;
    //return 0.0;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    //return vec3(0.0, 0.0, 0.0);
}
// ----------------------------------------------------------------------------
void main()
{
    vec3  albedo    = pow(texture(u_AlbedoMap, v_TexCoords).rgb, vec3(2.2));
    float metallic  = texture(u_MetallicMap, v_TexCoords).r;
    float roughness = texture(u_RoughnessMap, v_TexCoords).r*u_RoughnessScale;
    float ao        = texture(u_AOMap, v_TexCoords).r;

    vec3 N = getNormalFromMap();
    vec3 V = normalize(u_CameraPosition - v_WorldPos);

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    vec3 F0 = vec3(0.04);
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    for(int i = 0; i < 1; ++i)
    {
        // calculate per-light radiance
        vec3 L = normalize(u_LightPosition - v_WorldPos);
        vec3 H = normalize(V + L);
        float distance = length(u_LightPosition - v_WorldPos);
        float attenuation = 1.0 / (distance * distance) + 0.9;
        vec3 radiance = u_LightColor * attenuation;

        // Cook-Torrance BRDF
        float NDF = DistributionGGX(N, H, roughness);
        float G   = GeometrySmith(N, V, L, roughness);
        vec3 F    = fresnelSchlick(max(dot(H, V), 0.0), F0);

        vec3 nominator    = NDF * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001; // 0.001 to prevent divide by zero.
        vec3 specular = nominator / denominator;

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
    vec3 ambient = vec3(0.03) * albedo * ao;

    vec3 color = ambient + Lo;

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/u_Exposure));

    FragColor = vec4(color, 1.0);
    //FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}