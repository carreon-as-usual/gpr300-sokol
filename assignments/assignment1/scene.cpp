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

    glCreateFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    {  
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
    }

    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer not complete \n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Scene::~Scene()
{
    glDeleteFramebuffers(1, &framebuffer);
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

void Scene::Render(void)
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindFramebuffer(GL_FRAMEBUFFER, 1);
    {
    const auto view_proj = camera.Projection() * camera.View();

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
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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

    ImGui::Image(
        (void*)(intptr_t)fboTexture,
        ImVec2(400, 300),
        ImVec2(0, 1), ImVec2(1, 0));

    /* build debug ui here */

    ImGui::End();
}