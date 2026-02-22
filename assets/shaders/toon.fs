#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
  vec3 position;
  vec3 color;
};

 struct Palette{
  vec3 color1;
  vec3 color2;
 };

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D zatoon;

uniform Palette pal;

uniform vec3 camera;
uniform vec3 color;

uniform Light light;

vec3 toonshading(vec3 normal, vec3 frag_pos, Light light) {
  vec3 light_dir = normalize(light.position - frag_pos);
  float ndotl = (dot(normal, light_dir) + 1.0) * 0.5;

  vec3 gradient = texture(zatoon, vec2(ndotl, ndotl)).rgb;
  vec3 out_color = mix(pal.color1, pal.color2, gradient);

  return out_color * gradient;
}

void main()
{
  vec3 lighting = toonshading(vs_normal, vs_position, light);
  FragColor = vec4(lighting, 1.0);
}