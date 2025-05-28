#version 140
in vec3 position;
in vec3 normal;
in vec2 textureCoords;

out vec3 position_v;
out vec3 normal_v;
out vec2 textureCoords_v;
smooth out float fogFactor_v; 

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

float fogFactor(float fogCoord)
{
	float factor = exp(-0.8*fogCoord);
	factor = 1-clamp(factor,0.0,1.0);
	return factor;
};

void main()
{
    position_v		= vec3(model * vec4(position, 1.0));
    normal_v		= mat3(transpose(inverse(model))) * normal;  
    textureCoords_v = textureCoords;

	vec4 pos = projection * view * vec4(position_v, 1.0);
	float fogCoord = abs(pos.z / pos.w);
	fogFactor_v = fogFactor(fogCoord);
    
    gl_Position		= projection * view * vec4(position_v, 1.0);
}