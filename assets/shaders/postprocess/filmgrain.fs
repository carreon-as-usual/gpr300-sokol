#version 410

out vec4 FragColor;

in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D screen;

const float grain_amount = 0.05;
const float grain_size = 1.0;

void main()
{
  // https://godotshaders.com/shader/film-grain-shader/
  vec3 color = texture(screen, vs_texcoord).rgb;
  float noise = (fract(sin(dot(vs_texcoord, vec2(12.9898, 78.233))) * 43758.5453) - 0.5) * 2.0;
  color.rgb += noise * grain_amount * grain_size;
  FragColor = vec4(color, 1.0);
}