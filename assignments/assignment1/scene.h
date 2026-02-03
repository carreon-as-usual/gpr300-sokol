#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"
#include "batteries/materials.h"

// ew
#include "ew/model.h"
#include "ew/shader.h"
#include "ew/texture.h"

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Debug(void);

  private:
    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Shader> toonShading;
    std::unique_ptr<ew::Texture> texture;

    batteries::light_t light;
    batteries::material_t material;

    struct {
      glm::vec3 color1 = {0.0f, 1.0f, 0.5f};
      glm::vec3 color2 = {1.0f, 0.5f, 0.0f};
    } palette;

    unsigned int framebuffer;
    unsigned int fboTexture;
};
