﻿#include "mkpch.h"
#include "WindowsWindow.h"
#include "Mashenka/Events/ApplicationEvent.h"
#include "Mashenka/Events/MouseEvent.h"
#include "Mashenka/Events/KeyEvent.h"

namespace Mashenka
{
    // Check if glfw is initialized, shared by all instance to make sure glfw is initialized once only
    // As glfwinit() should only be called once during the execution of a program
    static bool s_GLFWInitialized = false;

    // Defined a Error log function
    static void GLFWErrorCallback(int error, const char* description)
    {
        MK_CORE_ERROR("GLFW Error ({0}): {1}", error, description);
    }

    Window* Window::Create(const WindowProps& props)
    {
        return new WindowsWindow(props);
    }

    // Using Init and Shutdown to organize and potentially reuse code when other ways to construct or destruct exists
    // It is also friendly to error handling as constructors and destructors are not allowed to throw exceptions or error codes
    WindowsWindow::WindowsWindow(const WindowProps& props)
    {
        Init(props);
    }

    WindowsWindow::~WindowsWindow()
    {
        Shutdown();
    }

    void WindowsWindow::Init(const WindowProps& props)
    {
        m_Data.Title = props.Title;
        m_Data.Width = props.Width;
        m_Data.Height = props.Height;

        MK_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

        if (!s_GLFWInitialized)
        {
            //TODO: glfwTerminate on system shutdown
            int success = glfwInit();
            MK_CORE_ASSERT(success, "Could not initialize GLFW!");
            // Error callback
            glfwSetErrorCallback(GLFWErrorCallback);

            s_GLFWInitialized = true;
        }

        m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, m_Data.Title.c_str(), nullptr, nullptr);
        glfwMakeContextCurrent(m_Window);

        /*
         * gladLoadGLLoader is a function provided by GLAD,
         * a loader-generator for OpenGL. It's used to load and initialize the OpenGL function pointers for the context
         * that's currently bound. In simpler terms, it sets up all the OpenGL functions so that
         * they can be used in your application.
         * glfwGetProcAddress is a function provided by GLFW that retrieves the address of an OpenGL function.
         * By casting it to GLADloadproc, you're telling GLAD to use this function to
         * load all the required OpenGL function addresses.
         */
        int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        MK_CORE_ASSERT(status, "Failed to initialize Glad!");
        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);

        // Set GLFW callbacks, callbacks will be called when the original functions are executed
        // But the callback functions might happen at a different time as they are on different layers
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action)
            {
            case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, 0);
                    data.EventCallback(event);
                    break;
                }
            case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
            case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, 1);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch(action)
            {
            case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
    }

    void WindowsWindow::Shutdown()
    {
        glfwDestroyWindow(m_Window);
    }
    
    void WindowsWindow::OnUpdate()
    {
        // Checks for any pending events and handle them, if not app or window will not respond
        glfwPollEvents();

        // Swap the back buffer with the front buffer as openGL normally render to off-screen buffer to avoid flickering
        glfwSwapBuffers(m_Window);
    }

    // Setup Vertical Sync for the GPU
    void WindowsWindow::SetVSync(bool enabled)
    {
        if (enabled)
            glfwSwapInterval(1);
        else
        {
            glfwSwapInterval(0);
        }

        m_Data.VSync = enabled;
    }

    bool WindowsWindow::IsVSync() const
    {
        return m_Data.VSync;
    }
}
