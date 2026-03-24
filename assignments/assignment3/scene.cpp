#include "scene.h"

// imgui
#include "imgui/imgui.h"
#include "imguizmo/imguizmo.h"

// glm
#include "glm/glm.hpp"
#include "glm/gtc/type_ptr.hpp"

// batteries
#include "batteries/opengl.h"

glm::mat4 suzanneMatrix = glm::mat4(1.0f);

struct {
    float bias = 0.005;
    float spacing = 2;
    int width = 3;
} debug;

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

void Scene::CreateFrameBuffer()
{
    int w = 800;
    int h = 600;
    glCreateFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    {  
        // create texture
        glGenTextures(1, &fbo_texture);
        glBindTexture(GL_TEXTURE_2D, fbo_texture);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  

        // create texture
        glGenTextures(1, &fbo_depth);
        glBindTexture(GL_TEXTURE_2D, fbo_depth);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fbo_depth, 0);

        // cleanup
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("Framebuffer not complete \n");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::CreateDepthBuffer()
{
    int w = 800;
    int h = 600;
    // framebuffer setup
    glCreateFramebuffers(1, &shadow_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
    {
        // create depth texture
        glGenTextures(1, &shadow_depth);
        glBindTexture(GL_TEXTURE_2D, shadow_depth);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, NULL);  
        // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);  
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_depth, 0);
    }
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        printf("Its not complete (depth) :(\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::InitializeInstanceData()
{
    auto size = (debug.width * debug.width - 1) * (debug.width * debug.width + 1);
    modelInstances.resize(size);
    auto i = 0;
    for(int x = -debug.width; x < debug.width; x++)
    {
        for(int y = -debug.width; y < debug.width; y++, i++)
        {
            auto position = glm::vec3(x * debug.spacing, 0, y * debug.spacing);
            auto matrix = glm::translate(suzanneMatrix, position);

            modelInstances[i] = matrix;
        }
    }
}

Scene::Scene()
{
    // models
    suzanne = std::make_unique<ew::Model>("assets/models/suzanne.obj", true);
    
    // shading
    toonShading = std::make_unique<ew::Shader>("assets/shaders/shadowmapping/default.vs", "assets/shaders/shadowmapping/toon.fs");
    blinnphong = std::make_unique<ew::Shader>("assets/shaders/shadowmapping/default.vs", "assets/shaders/shadowmapping/blinnphong.fs");

    depth = std::make_unique<ew::Shader>("assets/shaders/depth.vs", "assets/shaders/depth.fs");

    // textures
    toonTexture = std::make_unique<ew::Texture>("assets/textures/ZAtoon.png");
    blinnphongTexture  = std::make_unique<ew::Texture>("assets/textures/brick_color.jpg");

    shadingTypeIndex = 1;
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
    processes.push_back("pixelation");
    processes.push_back("filmgrain");

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

    CreateFrameBuffer();
    CreateDepthBuffer();
    InitializeInstanceData();

    // plane.load(ew::createPlane(100.0f, 100.0f, 10));

    // Initialize Instance Buffer
    glGenBuffers(1, &instanced_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, instanced_buffer);
    glBufferData(GL_ARRAY_BUFFER, 100 * sizeof(glm::mat4), &modelInstances[0], GL_STREAM_DRAW);
}

Scene::~Scene()
{
    glDeleteFramebuffers(1, &framebuffer);
    glDeleteFramebuffers(1, &shadow_fbo);
}

void Scene::Update(float dt)
{
    batteries::Scene::Update(dt);

    /* body */
}

void Scene::Render(void)
{
    glm::mat4 light_view_proj;
    // render scene from light
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
    {
        const auto light_proj = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
        const auto light_view = glm::lookAt(light.position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        light_view_proj = light_proj * light_view;

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        glEnable(GL_DEPTH_TEST);

        glViewport(0, 0, 800, 600);

        // glClearDepth(0.5);
        glClear(GL_DEPTH_BUFFER_BIT);

        depth->use();

        depth->setMat4("model", suzanneMatrix);
        depth->setMat4("light_view_proj", light_view_proj);

        suzanne->draw();
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // suzanne pipeline
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
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

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, shadow_depth);

        ew::Shader activeShading;

        if(shadingTypeIndex == 0)
        {
            blinnphong->use();

            blinnphong->setInt("texture0", 0);
            blinnphong->setInt("shadowmap", 2);

            blinnphong->setMat4("model", suzanneMatrix);
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

            blinnphong->setFloat("bias", debug.bias);

            blinnphong->setMat4("light_view_proj", light_view_proj);

            activeShading = *blinnphong;
        }

        if(shadingTypeIndex == 1) 
        {
            toonShading->use();

            toonShading->setInt("zatoon", 1);
            toonShading->setInt("shadowmap", 2);

            toonShading->setMat4("model", suzanneMatrix);
            toonShading->setMat4("view_proj", view_proj);
            toonShading->setVec3("camera", camera.position);

            toonShading->setVec3("light.position", light.position);
            toonShading->setVec3("light.color", light.color);

            toonShading->setVec3("pal.color1", light.color);
            toonShading->setVec3("pal.color2", ambient.color);
            
            toonShading->setFloat("bias", debug.bias);
    
            toonShading->setMat4("light_view_proj", light_view_proj);

            activeShading = *toonShading;
        }
        // draw suzanne
        // auto i = 0;
        // for(int x = -debug.width; x < debug.width; x++)
        // {
        //     for(int y = -debug.width; y < debug.width; y++, i++)
        //     {
        //         toonShading->setMat4("model", modelInstances[i]);
        //         blinnphong->setMat4("model", modelInstances[i]);
        //         suzanne->draw(100);
        //     }
        // }
        suzanne->draw(100);

        activeShading.setMat4("model", glm::translate(glm::mat4(1.0), glm::vec3(0.0, -2.0, 0.0)));
        plane.draw();
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
        glBindTexture(GL_TEXTURE_2D, fbo_texture);
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

    if(ImGui::SliderFloat("Spacing", &debug.spacing, 0.0, 10.0))
    {
        InitializeInstanceData();
    }
    if(ImGui::SliderInt("Width", &debug.width, 0, 1000))
    {
        InitializeInstanceData();
    }

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
    ImGui::SliderFloat("Shadow Bias", &debug.bias, 0, 0.05);
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
        ImGui::ColorEdit3("Color 1", &light.color.x);
        ImGui::ColorEdit3("Color 2", &ambient.color.x);
    }
        ImGui::Image(
        (void*)(intptr_t)fbo_texture,
        ImVec2(400, 300),
        ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Image(
        (void*)(intptr_t)shadow_depth,
        ImVec2(400, 300),
        ImVec2(0, 1), ImVec2(1, 0));

        ImGui::Image(
        (void*)(intptr_t)fbo_depth,
        ImVec2(400, 300),
        ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();
}