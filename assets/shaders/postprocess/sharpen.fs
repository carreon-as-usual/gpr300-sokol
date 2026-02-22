#version 410

out vec4 FragColor;

in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D screen;

//uniform float offset;
//uniform float strength;
const float offset = 1.0/300.0;
const float strength = 1.0/10.0;

const vec2 offsets[9] = vec2[](
  // 3 x 3 matrix
  // Left to Right

  // Top
  vec2(-offset, offset), 
  vec2(0.0, offset), 
  vec2(offset, offset),
  // Middle
  vec2(-offset,  0.0), 
  vec2(0.0, 0.0), 
  vec2(offset, 0.0),
  // Bottom
  vec2(-offset, -offset), 
  vec2(0.0, -offset), 
  vec2(offset, -offset)
);

const float kernel[9] = float[]
(
  0.0, -1.0, 0.0,
  -1.0, 5.0, -1.0,
  0.0, -1.0, 0.0
);

void main()
{
  vec3 color = vec3(0.0);
  for(int i = 0; i < 9; i++)
  {
    vec3 local = vec3(texture(screen, vs_texcoord.xy + offsets[i]));
    color += local * kernel[i] * strength;
  }
  FragColor = vec4(color, 1.0);
}