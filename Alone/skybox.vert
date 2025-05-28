#version 140

in vec2 screenCoords;
out vec3 textureCoords;
uniform mat4 inversePV;

void main() {
  gl_Position = vec4(screenCoords, 0.9999, 1.0);
  vec4 worldViewCoord = inversePV * vec4(screenCoords, 0.9999, 1.0);
  textureCoords = worldViewCoord.xyz / worldViewCoord.w; 
}