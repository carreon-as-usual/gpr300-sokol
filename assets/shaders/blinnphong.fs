#version 410

precision mediump float;

out vec4 FragColor;

struct Light {
  vec3 position;
  vec3 color;
};

// struct Material {
//   vec3 ambient;
//   vec3 diffuse;
//   vec3 specular;
//   vec3 shininess;
// };

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform vec3 camera;
uniform Light light;
uniform vec3 color;

uniform float alpha;

vec3 blinnphong(vec3 normal, vec3 frag_pos, Light light) {
  vec3 view_dir = normalize(camera - frag_pos);
  vec3 light_dir = normalize(light.position - frag_pos);
  vec3 reflect_dir = reflect(light_dir, vs_normal);
  vec3 half_dir = normalize(light_dir + view_dir);

  float diffuse = max(dot(normal, light_dir), 0.0);
  float specular = pow(max(dot(normal, half_dir), 0.0), alpha);
  //vec3 lighting = (material.ambient + (material.diffuse * diffuse) + (material.specular * specular)) * light.color;

  float PdotL = dot(frag_pos, light.position);
  return (diffuse + specular) * light.color;
}

void main()
{
  vec3 lighting = blinnphong(vs_normal, vs_position, light);
  vec3 object_color = vs_normal * 0.5 + 0.5;
  vec3 final_color = object_color * lighting;
  FragColor = vec4(final_color, 1.0);
}