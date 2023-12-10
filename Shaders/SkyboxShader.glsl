#type vertex
#version 330 core
layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec2 a_UV0;

uniform mat4 u_Projection;
uniform mat4 u_View;
uniform mat4 u_Model;

out vec2 v_UV0;
out vec3 v_LocalPos;

void main()
{
    v_LocalPos = a_Position;
    v_UV0 = a_UV0;
    gl_Position = u_Projection * u_View * u_Model * vec4(a_Position, 1.0);
}

#type fragment
#version 330 core
layout (location = 0) out vec4 Color;

in vec2 v_UV0;
in vec3 v_LocalPos;

uniform sampler2D u_EquirectangularMap;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(v_LocalPos)); // make sure to normalize localPos
    vec3 color = texture(u_EquirectangularMap, uv).rgb;
    Color = vec4(color, 1.0f);
}
