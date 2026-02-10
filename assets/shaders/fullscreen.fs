#version 410

out vec4 FragColor;

in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D screen;

void main()
{
  vec3 color = texture(screen, vs_texcoord).rgb;
  FragColor = vec4(color, 1.0);
}