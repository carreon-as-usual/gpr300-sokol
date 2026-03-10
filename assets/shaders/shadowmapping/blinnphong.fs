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
in vec4 vs_light_proj_pos;

uniform sampler2D texture0;
uniform sampler2D shadowmap;

uniform vec3 camera;
uniform vec3 color;

uniform Light light;
uniform Ambient ambient;
uniform Material material;
uniform float bias;

float shadowCalculation(vec4 frag_pos_light_space)
{
  vec3 proj_coords = frag_pos_light_space.xyz / frag_pos_light_space.w;
  proj_coords = proj_coords * 0.5 + 0.5;

  float closest_depth = texture(shadowmap, proj_coords.xy).r;
  float current_depth = proj_coords.z;

  vec3 normal = vs_normal;
  vec3 light_dir = normalize(light.position - vs_position);

  float shadow = 0.0;
  vec2 texel_offset = 1.0 /  textureSize(shadowmap, 0);

  for(int x = -1; x <= 1; ++x)
  {
      for(int y = -1; y <= 1; ++y)
      {
          vec2 uv = proj_coords.xy + vec2(x * texel_offset.x, y * texel_offset.y);
          shadow += (current_depth - bias) > texture(shadowmap,uv).r  ? 1.0 : 0.0;        
      }    
  }
  shadow /= 9.0;

  return shadow;
}

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
  float shadow = shadowCalculation(vs_light_proj_pos);
  vec3 lighting = blinnphong(vs_normal, vs_position, light, material) + ambient.color * ambient.intensity * material.ambient;
  lighting *= (1.0 - shadow);
  vec3 object_color = texture(texture0, vs_texcoord).rgb;
  vec3 final_color = object_color * lighting;
  FragColor = vec4(final_color, 1.0);
}