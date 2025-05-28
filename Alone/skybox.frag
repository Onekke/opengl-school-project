#version 140

in vec3 textureCoords;
out vec4 color;
uniform samplerCube textureSampler;
uniform float fog;

void main() {
	color = texture(textureSampler, textureCoords);
	float height = abs(1.0 - textureCoords.y/400.0);
	color = mix(color,vec4(0.6f,0.5f,0.4f,0.3f),height*fog);
}
