#type vertex
#version 430 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec3 outPos;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec2 outUV;

uniform mat4 MVP;

void main()
{
    const vec4 kWorldPos = MVP * vec4(inPos, 1.0);
    gl_Position = kWorldPos;

    // TODO model
    outPos = inPos.xyz;
    outNormal = inNormal;
    outUV = inUV;
}

#type fragment
#version 430 core
layout(location = 0) in vec3 inPos;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;

layout(location = 0) out vec4 outColor;

#define EPSILON 1e-5
#define REGION_MAX_COUNT 8

struct Region
{
    float scale;            ///< texture scale
    int texIndex;           ///< texture index in the texture array
    float blendStrength;
    float startHeight;
    vec3 tint;              ///< rgb: tint, a: tintStrength
};

uniform sampler2DArray texArray;
uniform sampler2D land;

uniform int u_RegionCount;
uniform float u_MinHeight;
uniform float u_MaxHeight;

uniform Region regions[REGION_MAX_COUNT];

float InverseLerp(float a, float b, float x) {
    return clamp( (x-a) / (b-a), 0.0, 1.0);
}

vec3 GetTriplanarMapping(const in vec3 kBlendAxis, const in float kScale,
                         const in int kTexIndex)
{
    const vec3 kScaledPos = inPos / kScale;

    return texture(texArray, vec3(kScaledPos.yz, kTexIndex) ).rgb * kBlendAxis.x +
           texture(texArray, vec3(kScaledPos.xz, kTexIndex) ).rgb * kBlendAxis.y +
           texture(texArray, vec3(kScaledPos.xy, kTexIndex) ).rgb * kBlendAxis.z;
}

void main()
{
    vec3 blendAxis = abs(inNormal);
    blendAxis /= blendAxis.x + blendAxis.y + blendAxis.z;

    const float kHeightPercent = InverseLerp(u_MinHeight, u_MaxHeight, inPos.y);

    vec3 color = vec3(0);
    const int kRegionCount = min(u_RegionCount, REGION_MAX_COUNT);

    for (int i = 0; i < kRegionCount; ++i)
    {
        const Region region = regions[i];

        const vec3 kTextureColor = GetTriplanarMapping(blendAxis, region.scale, region.texIndex);
        //vec3 kTextureColor = vec3(1.0);
        const vec3 kBaseColor = mix(kTextureColor, region.tint.rgb, 0.5);
        //const vec3 kBaseColor = vec3(1.0);

        const float kBlendStrengthHalf = region.blendStrength/2;
        const float kWeight = InverseLerp(-kBlendStrengthHalf - EPSILON,
                                           kBlendStrengthHalf,
                                           kHeightPercent - region.startHeight);
        color = mix(color, kBaseColor, kWeight);
        //color = regions[1].tint;
    }

    //color = ComputeLighting(color);

    //color = pow(color, vec3(0.4545));
    outColor = vec4(color, 1.0);
    //outColor = texture(land, inUV);
}