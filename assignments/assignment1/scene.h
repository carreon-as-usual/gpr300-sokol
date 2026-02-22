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
    std::unique_ptr<ew::Shader> blinnphong;
    std::unique_ptr<ew::Texture> toonTexture;
    std::unique_ptr<ew::Texture> blinnphongTexture;
    std::vector<std::string> shadingTypes;
    int shadingTypeIndex;

    std::unique_ptr<ew::Shader> postprocess;

    std::vector<std::string> processes;
    int processIndex;

    batteries::light_t light;
    batteries::ambient_t ambient;
    batteries::material_t material;

    struct {
      glm::vec3 color1 = {0.3f, 1.0f, 0.5f};
      glm::vec3 color2 = {0.0f, 0.5f, 1.0f};
    } palette;

    unsigned int framebuffer;
    unsigned int fboTexture;
    unsigned int fboDepth;
};
