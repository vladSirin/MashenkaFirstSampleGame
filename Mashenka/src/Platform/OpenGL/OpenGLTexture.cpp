﻿#include "mkpch.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include <stb_image.h>
#include <glad/glad.h>

namespace Mashenka
{
    // Constructor
    OpenGLTexture2D::OpenGLTexture2D(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height)
    {
        m_InternalFormat = GL_RGBA8;
        m_DataFormat = GL_RGBA;

        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID); // generate the texture, renderID is generated by OpenGL with glGenTextures
        glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height); // storage for the texture, we only need 1 level, so we use 1

        // Set the texture parameters, for min filter we use GL_LINEAR, for mag filter we use GL_NEAREST
        // Explanation: https://www.khronos.org/opengl/wiki/Sampler_Object
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    // Constructor
    OpenGLTexture2D::OpenGLTexture2D(const std::string& path)
        : m_Path(path)
    {
        // load the image
        int width, height, channels;
        stbi_set_flip_vertically_on_load(1); // flip the image vertically
        stbi_uc* data = nullptr;
        {
            // Explanation: https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture
            data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        }
        MK_CORE_ASSERT(data, "Failed to load image!")

        // set the width and height, format
        m_Width = width;
        m_Height = height;

        // Set the format
        GLenum internalFormat = 0, dataFormat = 0;
        if (channels == 4)
        {
            internalFormat = GL_RGBA8;
            dataFormat = GL_RGBA;
        }
        else if (channels == 3)
        {
            internalFormat = GL_RGB8;
            dataFormat = GL_RGB;
        }

        m_InternalFormat = internalFormat;
        m_DataFormat = dataFormat;

        // ----------------- Create the texture -----------------
        // Explanation: https://www.khronos.org/opengl/wiki/Texture_Storage
        // Explanation: https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture

        // Generate the texture, renderID is generated by OpenGL with glGenTextures
        glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
        // Storage for the texture, we only need 1 level, so we use 1
        glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

        // Set the texture parameters, for min filter we use GL_LINEAR, for mag filter we use GL_NEAREST
        // Explanation: https://www.khronos.org/opengl/wiki/Sampler_Object
        glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Set the texture parameters for wrap, we use GL_REPEAT
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
        

        // Set the texture parameters
        // SubImage2D: https://www.khronos.org/opengl/wiki/GLAPI/glTexSubImage2D
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    OpenGLTexture2D::~OpenGLTexture2D()
    {
        glDeleteTextures(1, &m_RendererID);
    }

    void OpenGLTexture2D::SetData(void* data, uint32_t size)
    {
        //setup the data for the texture created on runtime
        uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3; // bytes per pixel
        MK_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");

        // subimage2d is used to set the data of the texture, we use 0 for the level, 0 for the xoffset and yoffset, and the width and height of the texture
        glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data); 
    }

    void OpenGLTexture2D::Bind(uint32_t slot) const
    {
        glBindTextureUnit(slot, m_RendererID);
    }


}
