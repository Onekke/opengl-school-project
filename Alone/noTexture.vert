#version 140
in vec3 position_v;
in vec3 normals_v;

out vec3 position_f;
out vec3 normals_f;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	position_f  = vec3(model * vec4(position_v, 1.0));
	normals_f   = mat3(transpose(inverse(model))) * normals_v;

	gl_Position = projection * view * vec4(position_f, 1.0);
}