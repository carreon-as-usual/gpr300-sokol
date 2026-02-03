#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
  vec3 position;
  vec3 color;
};

 struct Material {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   float shininess;
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
uniform Material material;

uniform float alpha;

vec3 toonshading(vec3 normal, vec3 frag_pos, Light light, Material material) {
  vec3 view_dir = normalize(camera - frag_pos);
  vec3 light_dir = normalize(light.position - frag_pos);
  vec3 reflect_dir = reflect(light_dir, vs_normal);
  vec3 half_dir = normalize(light_dir + view_dir);

  float ndotl = (dot(normal, light_dir) + 1.0) * 0.5;
  float ndoth = pow(max(dot(normal, half_dir), 0.0), material.shininess);

  vec3 gradient = texture(zatoon, vec2(ndotl, ndotl)).rgb;
  vec3 out_color = mix(pal.color1, pal.color2, gradient);

  return out_color * gradient;
}

void main()
{
  vec3 lighting = toonshading(vs_normal, vs_position, light, material);
  FragColor = vec4(lighting, 1.0);
}