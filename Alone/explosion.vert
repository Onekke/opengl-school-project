#version 140

uniform mat4 PVM;
in vec3 position;
in vec2 textureCoords;

smooth out vec2 textureCoords_v; 

void main() {
  gl_Position = PVM * vec4(position, 1);
  textureCoords_v = textureCoords;
}
