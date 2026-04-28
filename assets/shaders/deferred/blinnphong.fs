#version 410

struct Light {
  vec3 position;
  vec3 color;
  float radius;
};

in vec2 vs_texcoord;

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;
uniform Light light;
uniform vec3 camera_position;

out vec4 FragLighting;

vec3 blinnphong(vec3 normal, vec3 frag_pos, Light light, vec4 material) {
  vec3 view_dir = normalize(camera_position - frag_pos);
  vec3 light_dir = normalize(light.position - frag_pos);
  vec3 reflect_dir = reflect(light_dir, normal);
  vec3 half_dir = normalize(light_dir + view_dir);

  float ndotl = max(dot(normal, light_dir), 0.0);
  float ndoth = max(dot(normal, half_dir), 0.0);

  vec3 diffuse = ndotl * vec3(material.g);
  vec3 specular = pow(ndoth, material.a * 128.0) * vec3(material.b);

  return (diffuse + specular) * light.color;
}

float attenuateExponential(float distance, float radius){
	float i = clamp(1.0 - pow(distance/radius,4.0),0.0,1.0);
	return i * i;	
}


void main()
{
  vec2 uv = gl_FragCoord.xy / vec2(800, 600);

  vec3 position = texture(g_position, uv).rgb;
  vec3 normal = texture(g_normal, uv).rgb;
  vec4 material = texture(g_material, uv).rgba;
  float distance = length(light.position - position);
  vec3 finalLightColor = blinnphong(normal, position, light, material) * attenuateExponential(distance, light.radius);

  FragLighting = vec4(finalLightColor, 1.0);
}