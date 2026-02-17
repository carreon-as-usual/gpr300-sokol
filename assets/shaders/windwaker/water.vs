#version 410

// attributes
layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texcoord;

// uniforms
uniform mat4 view_proj;
uniform mat4 model;

uniform float time;

// varyings
out vec3 vs_position;
out vec3 vs_normal;
out vec2 vs_texcoord;

float scale = 0.5;

float calculateSurface(float x, float z)
{
  float y = 0.0;

  y += (sin(x * 1.0 / scale + time * 1.0) + sin(x * 2.3 / scale + time * 1.5) + sin(x * 3.5 / scale + time * 1.5));
  y += (sin(z * 0.2 / scale + time * 1.0) + sin(z * 1.8 / scale + time * 1.8) + sin(z * 2.0 / scale + time * 2.5));

  return y;
}

void main()
{
  vs_position = in_position;
  vs_normal = transpose(inverse(mat3(model))) * in_normal;
  vs_texcoord = in_texcoord;

  vec3 position = in_position;
  position.y += calculateSurface(position.x, position.z) * 1.5;

  gl_Position = view_proj * model * vec4(position, 1.0);
}