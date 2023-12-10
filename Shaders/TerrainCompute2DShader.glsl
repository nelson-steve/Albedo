	#type compute
#version 430 core

#define PI 3.1415926535897932384626433832795

#define GROUP_SIZE 16

layout (local_size_x = GROUP_SIZE, local_size_y = GROUP_SIZE) in;

layout (rgba32f, binding = 0) uniform image2D heightmap;
//layout (r32f, binding = 1) uniform image2D intermediate_height_buffer;

shared float workGroupHeight[GROUP_SIZE][GROUP_SIZE];

layout(std140) uniform GenerationUniform
{
	dvec2 tileOffset;
	//double tileScale;
	//double pad;
	//mat4 normalTransform;
	//dmat4 pointTransform;
	//ivec2 gridDimensions;
};

float rand(vec2 c){
	return fract(sin(dot(c.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

float noise(vec2 p, float freq ){
	float unit = 1024/freq;
	vec2 ij = floor(p/unit);
	vec2 xy = mod(p,unit)/unit;
	//xy = 3.*xy*xy-2.*xy*xy*xy;
	xy = .5*(1.-cos(PI*xy));
	float a = rand((ij+vec2(0.,0.)));
	float b = rand((ij+vec2(1.,0.)));
	float c = rand((ij+vec2(0.,1.)));
	float d = rand((ij+vec2(1.,1.)));
	float x1 = mix(a, b, xy.x);
	float x2 = mix(c, d, xy.x);
	return mix(x1, x2, xy.y);
}

float pNoise(vec2 p, int res){
	float persistance = .5;
	float n = 0.;
	float normK = 0.;
	float f = 4.;
	float amp = 1.;
	int iCount = 0;
	for (int i = 0; i<70; i++){
		n+=amp*noise(p + vec2(tileOffset), f);
		f*=2.;
		normK+=amp;
		amp*=persistance;
		if (iCount == res) break;
		iCount++;
	}
	float nf = n/normK;
	return nf*nf*nf*nf;
}

void main()
{
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
	
	workGroupHeight[gl_LocalInvocationID.x][gl_LocalInvocationID.y] = float(gl_GlobalInvocationID.x/GROUP_SIZE);

    vec2 pixel = vec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y);
    int res = 64;
    float persist = .5;
    float freq = 4.;
    float amp = 1.;
    int oct = 50;
    //float n = pNoise(pixel, res, persist, freq, amp, oct);
    float n = pNoise(pixel, 256);
	vec4 finalColor = vec4(n, n, n, 1.0);
	//vec4 finalColor = vec4(1.0, 0.0, 0.0, 1.0);
	
	//Store the final normal and height maps
	imageStore(heightmap, ivec2(gl_GlobalInvocationID.x, gl_GlobalInvocationID.y), finalColor);
}