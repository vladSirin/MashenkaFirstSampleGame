﻿#include "mkpch.h"
#include "Mashenka/Renderer/Renderer2D.h"

#include "Mashenka/Renderer/VertexArray.h"
#include "Mashenka/Renderer/Shader.h"
#include "Mashenka/Renderer/RenderCommand.h"
// #include "Platform/OpenGL/OpenGLShader.h", but we can't include it here because it will cause a circular dependency
#include <glm/gtc/matrix_transform.hpp> // for glm::mat4

namespace Mashenka
{
    // Initialize the scene data
    struct Render2DStorage
    {
        Ref<VertexArray> QuadVertexArray;
        Ref<Shader> TextureShader;
        Ref<Texture2D> WhiteTexture;
    };

    // Initialize the scene data
    static Render2DStorage* s_Data;
    
    void Renderer2D::Init()
    {
        MK_PROFILE_FUNCTION(); // Profiling
        // Initialize the renderer API
        s_Data = new Render2DStorage();
        
        s_Data->QuadVertexArray = VertexArray::Create();
        
        float squareVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 0
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 1
             0.5f,  0.5f, 0.0f, 1.0f, 1.0f, // 2
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f  // 3
        };

        // Create the vertex buffer
        Ref<VertexBuffer> squareVB = VertexBuffer::Create(squareVertices, sizeof(squareVertices));
        BufferLayout layout = {
            { ShaderDataType::Float3, "a_Position" },
            { ShaderDataType::Float2, "a_TexCoord" }
        };
        squareVB->SetLayout(layout);
        s_Data->QuadVertexArray->AddVertexBuffer(squareVB);

        // Create the index buffer
        uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
        Ref<IndexBuffer> squareIB = IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t));
        s_Data->QuadVertexArray->SetIndexBuffer(squareIB);

        // Create the shaders
        s_Data->WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff; // white
        s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
        
        s_Data->TextureShader = Shader::Create("assets/shaders/Texture.glsl");
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetInt("u_Texture", 0); // set the texture slot to 0, because we will always use slot 0 for textures
    }

    void Renderer2D::Shutdown()
    {
        MK_PROFILE_FUNCTION(); // Profiling
        delete s_Data;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        MK_PROFILE_FUNCTION(); // Profiling
        s_Data->TextureShader->Bind();
        s_Data->TextureShader->SetMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
    }

    void Renderer2D::EndScene()
    {
        MK_PROFILE_FUNCTION(); // Profiling
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, float angle,const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({ position.x, position.y, 0.0f }, angle,  size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, float angle, const glm::vec2& size, const glm::vec4& color)
    {
        MK_PROFILE_FUNCTION(); // Profiling
        s_Data->TextureShader->SetFloat4("u_Color", color); // set the color
        s_Data->WhiteTexture->Bind(); // bind the texture
        
        // Create the transformation matrix
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)* glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f))* glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
        s_Data->TextureShader->SetMat4("u_Transform", transform); // set the transformation matrix
        
        // Draw the quad
        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, float angle, const glm::vec2& size, const Ref<Texture2D>& texture)
    {
        DrawQuad({ position.x, position.y, 0.0f },angle , size, texture);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, float angle, const glm::vec2& size, const Ref<Texture2D>& texture)
    {
        MK_PROFILE_FUNCTION(); // Profiling
        s_Data->TextureShader->SetFloat4("u_Color", glm::vec4(1.0f)); // set the color to white
        texture->Bind(); // bind the texture

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)* glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f))* glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));
        s_Data->TextureShader->SetMat4("u_Transform", transform);

        texture->Bind();
        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
    }
}
