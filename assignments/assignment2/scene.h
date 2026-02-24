#pragma once

// batteries
#include "batteries/scene.h"
#include "batteries/lights.h"
#include "batteries/materials.h"

// ew
#include "ew/model.h"
#include "ew/shader.h"
#include "ew/texture.h"
#include "ew/mesh.h"
#include "ew/procGen.h"

class Scene final : public batteries::Scene
{
  public:
    Scene();
    virtual ~Scene();

    void Update(float dt);
    void Render(void);
    void Debug(void);

  private:
    void CreateFrameBuffer();
    void CreateDepthBuffer();

    std::unique_ptr<ew::Model> suzanne;
    std::unique_ptr<ew::Shader> toonShading;
    std::unique_ptr<ew::Shader> blinnphong;
    std::unique_ptr<ew::Texture> toonTexture;
    std::unique_ptr<ew::Texture> blinnphongTexture;
    std::vector<std::string> shadingTypes;
    int shadingTypeIndex;

    ew::Mesh plane;

    std::unique_ptr<ew::Shader> postprocess;
    std::unique_ptr<ew::Shader> depth;

    std::vector<std::string> processes;
    int processIndex;

    batteries::light_t light;
    batteries::ambient_t ambient;
    batteries::material_t material;

    // framebuffer
    unsigned int framebuffer;
    unsigned int fbo_texture;
    unsigned int fbo_depth;

    // shadowbuffer
    unsigned int shadow_fbo;
    unsigned int shadow_depth;
};
