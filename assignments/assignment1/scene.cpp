#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"


struct FullScreenQuad
{
    unsigned int vao;
    unsigned int vbo;

    void Initialize()
    {
        float vertices[] = {
            //pox (x, y) texCoord (u, v)

            // Triangle 1
            -1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f,

            // Triangle 2
            -1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f,
        };

        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &vbo);

        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(sizeof(float) * 2));

        glBindVertexArray(0); // always last
    }
} fullscreen_quad;

Scene::Scene()
{
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj");
    toonShading = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/toon.fs");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/default.vs", "assets/shaders/blinnphong.fs");
    toonTexture = std::make_unique<ew::Texture>("assets/textures/ZAtoon.png");
    blinnphongTexture  = std::make_unique<ew::Texture>("assets/textures/brick_color.jpg");

    shadingTypeIndex = 0;
    shadingTypes.push_back("blinnphong");
    shadingTypes.push_back("toon");

    processIndex = 0;
    processes.push_back("none");
    processes.push_back("grayscale");
    processes.push_back("blur");
    processes.push_back("invert");
    processes.push_back("sharpen");
    processes.push_back("edgedetection");
    processes.push_back("chromaticabberation");

    postprocess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocess/" + processes[processIndex] +".fs");

    light = {
        .brightness = 1.0f,
        .color = {1.0f, 0.0f, 1.0f},
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

    fullscreen_quad.Initialize();

    glCreateFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    {  
        // create texture
        glGenTextures(1, &fboTexture);
        glBindTexture(GL_TEXTURE_2D, fboTexture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

        // create texture
        glGenTextures(1, &fboDepth);
        glBindTexture(GL_TEXTURE_2D, fboDepth);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fboDepth, 0);

        // cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
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
    // suzanne pipeline
    glBindFramebuffer(GL_FRAMEBUFFER, 1);
    {
        const auto view_proj = camera.Projection() * camera.View();
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, blinnphongTexture->getID());

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, toonTexture->getID());

        if(shadingTypeIndex == 0)
        {
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
        }

        if(shadingTypeIndex == 1) 
        {
            toonShading->use();

            toonShading->setInt("zatoon", 1);

            toonShading->setMat4("model", glm::mat4(1.0f));
            toonShading->setMat4("view_proj", view_proj);
            toonShading->setVec3("camera", camera.position);

            toonShading->setVec3("light.position", light.position);
            toonShading->setVec3("light.color", light.color);

            toonShading->setVec3("pal.color1", palette.color1);
            toonShading->setVec3("pal.color2", palette.color2);
        }
        // draw suzanne
        suzanne->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // post processing pipeline
    {
        // render fullscreen quad
        postprocess->use();
        postprocess->setInt("screen", 0);

        //postprocess->setFloat("strength", 0.1f);

        // fullscreen pipeline
        glDisable(GL_DEPTH_TEST);

        // default framebuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // draw fullscreenquad
        glBindVertexArray(fullscreen_quad.vao);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, fboTexture);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
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

    // ImGui::Checkbox("Paused", &time.paused);
    // ImGui::SliderFloat("Time Factor", &time.factor, 0.0f, 10.0f);

    if (ImGui::BeginCombo("Effect", processes[processIndex].c_str()))
    {
        for (auto n = 0; n < processes.size(); ++n)
        {
            auto is_selected = (processes[processIndex] == processes[n]);
            if (ImGui::Selectable(processes[n].c_str(), is_selected))
            {
                processIndex = n;
                postprocess = std::make_unique<ew::Shader>("assets/shaders/fullscreen.vs", "assets/shaders/postprocess/" + processes[processIndex] +".fs");
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    if (ImGui::BeginCombo("Shading Type", shadingTypes[shadingTypeIndex].c_str()))
    {
        for (auto n = 0; n < shadingTypes.size(); ++n)
        {
            auto is_selected = (shadingTypes[shadingTypeIndex] == shadingTypes[n]);
            if (ImGui::Selectable(shadingTypes[n].c_str(), is_selected))
            {
                shadingTypeIndex = n;
            }
            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }
        }
        ImGui::EndCombo();
    }
    if(shadingTypeIndex == 0)
    {
        ImGui::ColorEdit3("Light Color", &light.color.x);

        ImGui::ColorEdit3("Ambient Color", &ambient.color.x);
        ImGui::SliderFloat("Ambient Intensity", &ambient.intensity, 0.0, 1.0);

        ImGui::SliderFloat3("Material Ambient", &material.ambient.x, 0.0, 1.0);
        ImGui::SliderFloat3("Material Diffuse", &material.diffuse.x, 0.0, 1.0);
        ImGui::SliderFloat3("Material Specular", &material.specular.x, 0.0, 1.0);
        ImGui::SliderFloat("Material Shininess", &material.shininess, 0.0, 1.0);
    }
    if(shadingTypeIndex == 1)
    {
        ImGui::ColorEdit3("Color 1", &palette.color1.x);
        ImGui::ColorEdit3("Color 2", &palette.color2.x);
    }
    /*
    ImGui::Image(
        (void*)(intptr_t)fboTexture,
        ImVec2(400, 300),
        ImVec2(0, 1), ImVec2(1, 0));

    ImGui::Image(
        (void*)(intptr_t)fboDepth,
        ImVec2(400, 300),
        ImVec2(0, 1), ImVec2(1, 0));
    */
    ImGui::End();
}