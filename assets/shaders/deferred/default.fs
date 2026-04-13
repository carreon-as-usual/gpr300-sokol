#version 410

out vec4 FragColor;

in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D albedo;
uniform sampler2D blinnphong;

void main()
{
  vec3 final_color = texture(albedo, vs_texcoord).rgb;
  vec3 lighting = texture(blinnphong, vs_texcoord).rgb;
  FragColor = vec4(final_color * lighting, 1.0);
}