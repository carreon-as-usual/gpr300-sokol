#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

struct{
    glm::vec3 waterColor{0.0f, 0.5f, 1.0f};
} debug;

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    water = std::make_unique<ew::Shader>("assets/shaders/doubledash/water.vs", "assets/shaders/doubledash/water.fs");
    texture = std::make_unique<ew::Texture>("assets/textures/brick_color.jpg");

    wave_tex = std::make_unique<ew::Texture>("assets/textures/doubledash/wave_tex.png");
    wave_spec = std::make_unique<ew::Texture>("assets/textures/doubledash/wave_spec.png");
    wave_warp = std::make_unique<ew::Texture>("assets/textures/doubledash/wave_warp.png");

    //camera.position = {3.5f, 2.5f, 4.0f};
    light = {
        .brightness = 5.0f,
        .color = {1.0f, 1.0f, 1.0f},
        .position = {1.0f, 1.0f, 1.0f},
    };
    material = {
        .ambient = {0.3f, 0.3f, 0.3f},
        .diffuse = {0.5f, 0.5f, 0.5f},
        .specular = {0.5f, 0.5f, 0.5f},
        .shininess = 1.0f,
    };
    ambient = {
        .intensity = 0.1f,
        .color = {1.0f, 1.0f, 1.0f},
    };
    
    plane.load(ew::createPlane(100.0f, 100.0f, 10));
}

Scene::~Scene()
{
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

void Scene::Render(void)
{
    const auto view_proj = camera.Projection() * camera.View();

    glClearColor(0.2f, 0.5f, 0.7f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, wave_tex->getID());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, wave_spec->getID());
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, wave_warp->getID());

    water->use();

    water->setFloat("time", time.absolute);

    water->setInt("wave_tex", 0);
    water->setInt("wave_spec", 1);
    water->setInt("wave_warp", 2);
    water->setVec3("water_color", debug.waterColor);

    water->setMat4("model", glm::mat4(1.0f));
    water->setMat4("view_proj", view_proj);
    water->setVec3("camera", camera.position);

    plane.draw();
}

void Scene::Debug(void)
{
    ImGuizmo::BeginFrame();
    ImGuizmo::SetDrawlist(ImGui::GetBackgroundDrawList());
    ImGuizmo::SetRect(0, 0, ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);

    glm::mat4 m{1.0f};
    auto *view = glm::value_ptr(camera.View());
    auto *proj = glm::value_ptr(camera.Projection());

    auto light_matrix = glm::translate(glm::mat4(1.0f), light.position);
    ImGuizmo::Manipulate(
        view,
        proj,
        ImGuizmo::TRANSLATE,
        ImGuizmo::WORLD,
        glm::value_ptr(light_matrix)
    );

    if(ImGuizmo::IsUsing())
    {
        light.position = glm::vec3(light_matrix[3]);
    }

    cameracontroller.Debug();

    ImGui::Begin("Controls", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Checkbox("Paused", &time.paused);
    ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);
    ImGui::ColorEdit3("Water Color", &debug.waterColor.x);
    /* build debug ui here */

    ImGui::End();
}