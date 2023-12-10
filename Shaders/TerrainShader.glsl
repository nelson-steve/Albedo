#type vertex
#version 430 core
layout (location = 0) in vec3 a_Position;

out float Height;
//out vec3 Position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    Height = a_Position.y;
    //Position = (view * model * vec4(a_Position, 1.0)).xyz;
    gl_Position = projection * view * model * vec4(a_Position, 1.0);

}

#type fragment
#version 430 core
out vec4 FragColor;

in float Height;

void main()
{    
    float h = (Height + 16)/32.0f;	// shift and scale the height into a grayscale value
    FragColor = vec4(h, h, h, 1.0);
}