#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
  vec3 position;
  vec3 color;
};

struct Ambient {
  float intensity;
  vec3 color;
};

struct Material {
   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
   float shininess;
 };

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D texture0;

uniform vec3 camera;
uniform vec3 color;

uniform Light light;
uniform Ambient ambient;
uniform Material material;

vec3 blinnphong(vec3 normal, vec3 frag_pos, Light light, Material material) {
  vec3 view_dir = normalize(camera - frag_pos);
  vec3 light_dir = normalize(light.position - frag_pos);
  vec3 reflect_dir = reflect(light_dir, vs_normal);
  vec3 half_dir = normalize(light_dir + view_dir);

  float ndotl = max(dot(normal, light_dir), 0.0);
  float ndoth = max(dot(normal, half_dir), 0.0);

  vec3 diffuse = ndotl * material.diffuse;
  vec3 specular = pow(ndoth, material.shininess * 128.0) * material.specular;

  return (diffuse + specular) * light.color;
}

void main()
{
  vec3 lighting = blinnphong(vs_normal, vs_position, light, material) + ambient.color * ambient.intensity * material.ambient;
  vec3 object_color = texture(texture0, vs_texcoord).rgb;
  vec3 final_color = object_color * lighting;
  FragColor = vec4(final_color, 1.0);
}