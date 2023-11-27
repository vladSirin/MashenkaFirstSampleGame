﻿#include "mkpch.h"
#include "OpenGLContext.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gl/GL.h>

namespace Mashenka
{
    OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
        : m_WindowHandle(windowHandle)
    {
        MK_CORE_ASSERT(windowHandle, "Window hanlde is null")
    }

    void OpenGLContext::Init()
    {
        /*
         * gladLoadGLLoader is a function provided by GLAD,
         * a loader-generator for OpenGL. It's used to load and initialize the OpenGL function pointers for the context
         * that's currently bound. In simpler terms, it sets up all the OpenGL functions so that
         * they can be used in your application.
         * glfwGetProcAddress is a function provided by GLFW that retrieves the address of an OpenGL function.
         * By casting it to GLADloadproc, you're telling GLAD to use this function to
         * load all the required OpenGL function addresses.
        */
        glfwMakeContextCurrent(m_WindowHandle); // Associate the context with current window, crucial step for next
        int status = gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress));
        MK_CORE_ASSERT(status, "Failed to initialize Glad!")

        // Logging the rendering info
        MK_CORE_INFO("OpenGL Info:");
        MK_CORE_INFO(" Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR))); //eg: Intel
        MK_CORE_INFO(" Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER))); //eg: RTX 4060
        MK_CORE_INFO(" Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION))); //eg: OPENGL Version

        // Enable OpenGL Debug Context
#ifdef MK_ENABLE_ASSERTS
        int versionMajor;
        int versionMinor;
        glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
        glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

        MK_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 5),
                       "Mashenka requires at least OpenGL version 4.5!");
#endif
    }

    void OpenGLContext::SwapBuffers()
    {
        glfwSwapBuffers(m_WindowHandle);
    }
}
