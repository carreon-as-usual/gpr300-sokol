#version 410

out vec4 FragColor;

// varyings
in vec3 vs_position;
in vec3 vs_normal;
in vec2 vs_texcoord;

uniform sampler2D wave_tex;
uniform sampler2D wave_spec;
uniform sampler2D wave_warp;
uniform vec3 camera;

uniform vec3 water_color;

uniform float time;

const float scale = 2;

void main()
{
  vec2 dir = vec2(1.0, 0.0);
  vec2 uv = vs_texcoord + vec2(time * dir);
  uv.x += 0.01 * sin(uv.x * 3.5 + time);
  uv.y += -0.15 * sin(uv.y * 1.5 + time);

  vec4 sample1 = texture(wave_tex, uv * 1.0);
  vec4 sample2 = texture(wave_tex, uv * 1.2);

  // warp
  vec2 warp_uv = vs_texcoord * scale;
  vec2 warp_scroll = vec2(0.5, 0.5) * time * 0.2;
  vec2 warp = texture(wave_warp, warp_uv + warp_scroll).xy;

  // albedo
  vec2 albedo_uv = vs_texcoord * scale;
  vec4 albedo = texture(wave_tex, albedo_uv + warp);

  vec3 final_color = water_color + vec3(albedo.a);

  // specular/shimmer
  vec2 spec_uv = vs_texcoord * scale;
  vec3 spec_smp1 = texture(wave_spec, spec_uv + vec2(0.5, 0.5) * time).rgb;
  vec3 spec_smp2 = texture(wave_spec, spec_uv + vec2(-0.3, -0.7) * time).rgb;
  vec3 spec = vec3(spec_smp1 + spec_smp2);

  // fresnel
  float fresnel = dot(normalize(camera), vec3(0.0, 1.0, 0.0));

  const vec3 k_bright = vec3(0.299, 0.587, 0.114);
  float brightness = dot(spec, k_bright);

  float lower = 0.3;
  float upper = 1.0;
  if(brightness <= lower || brightness > upper)
  {
    final_color = mix(final_color, final_color + spec, fresnel);
  }


  FragColor = vec4(final_color, 1.0);
}