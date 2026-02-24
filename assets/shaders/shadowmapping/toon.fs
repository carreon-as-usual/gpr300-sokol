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
in vec4 vs_light_proj_pos;

uniform sampler2D zatoon;
uniform sampler2D shadowmap;

uniform Palette pal;

uniform vec3 camera;
uniform vec3 color;

uniform Light light;


float shadowCalculation(vec4 frag_pos_light_space)
{
  float shadow = 1.0;
  // vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
  // float closest = texture(shadow, proj_coords.xy);
  // float current = proj_coords.z;
  return shadow;
}

vec3 toonShading(vec3 normal, vec3 frag_pos, Light light) 
{
  vec3 light_dir = normalize(light.position - frag_pos);
  float ndotl = (dot(normal, light_dir) + 1.0) * 0.5;

  vec3 gradient = texture(zatoon, vec2(ndotl, ndotl)).rgb;
  vec3 out_color = mix(pal.color1, pal.color2, gradient);

  return out_color * gradient;
}

void main()
{
  float shadow = shadowCalculation(vs_light_proj_pos);
  vec3 lighting = toonShading(vs_normal, vs_position, light);
  FragColor = vec4(lighting, 1.0);
}