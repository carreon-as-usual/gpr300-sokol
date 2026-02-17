#version 410

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D texture0;
uniform vec3 camera;

uniform vec3 water_color;

uniform float time;

const float speed = 0.2;

void main()
{
  vec2 dir = vec2(1.0, 0.0);
  vec2 uv = vs_texcoord + vec2(time * dir) * speed;
  uv.x += 0.01 * sin(uv.x * 3.5 + time);
  uv.y += -0.15 * sin(uv.y * 1.5 + time);

  vec4 sample1 = texture(texture0, uv * 1.0);
  vec4 sample2 = texture(texture0, uv * 1.2);

  vec3 object_color = vec3(sample1 * 0.75 - sample2 * 0.25);
  FragColor = vec4(object_color + water_color, 1.0);
}