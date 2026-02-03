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
    float alpha = 2;
} debug;

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    toonShading = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/toon.fs");
    texture = std::make_unique<ew::Texture>("assets/textures/ZAtoon.png");

    light = {
        .brightness = 1.0f,
        .color = {1.0f, 0.0f, 1.0f},
        .position = {1.0f, 1.0f, 1.0f},
    };
    material = {
        .ambient = {0.0f, 1.0f, 0.0f},
        .diffuse = {0.5f, 0.5f, 0.5f},
        .specular = {0.3f, 0.3f, 0.3f},
        .shininess = 1.0f,
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

    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture->getID());

    toonShading->use();

    toonShading->setInt("zatoon", 0);

    toonShading->setMat4("model", glm::mat4(1.0f));
    toonShading->setMat4("view_proj", view_proj);
    toonShading->setVec3("camera", camera.position);

    toonShading->setVec3("light.position", light.position);
    toonShading->setVec3("light.color", light.color);
    toonShading->setFloat("alpha", debug.alpha);
    
    toonShading->setVec3("material.ambient", material.ambient);
    toonShading->setVec3("material.diffuse", material.diffuse);
    toonShading->setVec3("material.specular", material.specular);
    toonShading->setFloat("material.shininess", material.shininess);

    toonShading->setVec3("pal.color1", palette.color1);
    toonShading->setVec3("pal.color2", palette.color2);

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
    
    ImGuizmo::DrawGrid(view, proj, glm::value_ptr(m), 100.0f);

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

    ImGui::DragFloat("Alpha", &debug.alpha, 0.1f, 0.0f, 8.0f);

    ImGui::ColorEdit3("Color 1", &palette.color1.x);
    ImGui::ColorEdit3("Color 2", &palette.color2.x);

    /* build debug ui here */

    ImGui::End();
}