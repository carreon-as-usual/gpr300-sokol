#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/blinnphong.fs");
    texture = std::make_unique<ew::Texture>("assets/textures/brick_color.jpg");

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
    glBindTexture(GL_TEXTURE_2D, texture->getID());

    blinnphong->use();

    blinnphong->setInt("texture0", 0);

    blinnphong->setMat4("model", glm::mat4(1.0f));
    blinnphong->setMat4("view_proj", view_proj);
    blinnphong->setVec3("camera", camera.position);

    blinnphong->setVec3("light.position", light.position);
    blinnphong->setVec3("light.color", light.color);

    blinnphong->setVec3("ambient.color", ambient.color);
    blinnphong->setFloat("ambient.intensity", ambient.intensity);
    
    blinnphong->setVec3("material.ambient", material.ambient);
    blinnphong->setVec3("material.diffuse", material.diffuse);
    blinnphong->setVec3("material.specular", material.specular);
    blinnphong->setFloat("material.shininess", material.shininess);

    // draw suzanne
    suzanne->draw();
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
    
    ImGui::ColorEdit3("Light Color", &light.color.x);

    ImGui::ColorEdit3("Ambient Color", &ambient.color.x);
    ImGui::SliderFloat("Ambient Intensity", &ambient.intensity, 0.0, 1.0);

    ImGui::SliderFloat3("Material Ambient", &material.ambient.x, 0.0, 1.0);
    ImGui::SliderFloat3("Material Diffuse", &material.diffuse.x, 0.0, 1.0);
    ImGui::SliderFloat3("Material Specular", &material.specular.x, 0.0, 1.0);
    ImGui::SliderFloat("Material Shininess", &material.shininess, 0.0, 1.0);
    /* build debug ui here */

    ImGui::End();
}