#version 140

uniform float time;         
uniform mat4 view;       
uniform sampler2D textureSampler;

smooth in vec3 position_v;   
smooth in vec2 textureCoords_v;   

out vec4 color_f;            

// there are 8 frames in the row, two rows total
uniform ivec2 pattern = ivec2(8, 2);
// one frame lasts 0.1s
uniform float frameDuration = 0.1f;


vec4 sampleTexture(int frame) {

  vec2 offset = vec2(1.0) / vec2(pattern);

  vec2 textureCoordsBase = textureCoords_v / vec2(pattern);
  vec2 textureCoords_f = textureCoordsBase + vec2(frame % pattern.x, /*pattern.y - 1 -*/ (frame / pattern.x)) * offset;

  return texture(textureSampler, textureCoords_f);
}

void main() {
  int frame = int(time / frameDuration);
  color_f = sampleTexture(frame);
}
