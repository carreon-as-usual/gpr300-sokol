#version 410

struct Light {
  vec3 position;
  vec3 color;
};

uniform sampler2D g_position;
uniform sampler2D g_normal;
uniform sampler2D g_albedo;
uniform sampler2D g_material;
uniform Light light;
uniform vec3 camera_position;

out vec4 FragLighting;

void main()
{
    FragLighting = new vec4(0.0, 0.3, 0.7, 1.0);
}