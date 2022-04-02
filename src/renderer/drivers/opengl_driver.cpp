#include "opengl_driver.h"
#include "tibsun_globals.h"
#include "debughandler.h"
#include "asserthandler.h"

#include <GL/glew.h>

#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>


OpenGLVideoDriver::OpenGLVideoDriver() :
    VideoDriver("OpenGL"),
    Window(nullptr),
    WindowTitle(),
    Windowed(true),          // Default to windowed mode.
    Borderless(false)
{
}


OpenGLVideoDriver::~OpenGLVideoDriver()
{
}


bool OpenGLVideoDriver::Create_Window(Wstring window_title, int width, int height, int refresh_rate, bool windowed, bool borderless)
{
    if (glfwInit() == 0) {
        DEBUG_ERROR("glfwInit failed in %s!\n", __FUNCTION__);
        return false;
    }

    WindowTitle = window_title;
    WindowWidth = width;
    WindowHeight = height;
    RefreshRate = refresh_rate;
    Windowed = windowed;
    Borderless = borderless;

    Create_Window_Internal();

    if (glewInit() != GLEW_OK) {
        DEBUG_ERROR("glewInit failed in %s!\n", __FUNCTION__);
        return false;
    }

    int glfw_ver_major = glfwGetWindowAttrib(Window, GLFW_CONTEXT_VERSION_MAJOR);
    int glfw_ver_minor = glfwGetWindowAttrib(Window, GLFW_CONTEXT_VERSION_MINOR);
    int glfw_ver_rev = glfwGetWindowAttrib(Window, GLFW_CONTEXT_REVISION);

    DEBUG_INFO("glfw - Major:%d Minor:%d Rev:%d\n", glfw_ver_major, glfw_ver_minor, glfw_ver_rev);
    
    /**
     *  Set the window callbacks.
     */
    //glfwSetWindowSizeCallback(Window, Window_Size_Callback);
    //glfwSetWindowIconifyCallback(Window, Window_Iconify_Callback);
    //glfwSetWindowMaximizeCallback(Window, window_maximize_callback);
    //glfwSetWindowFocusCallback(Window, window_focus_callback);
    //glfwSetWindowRefreshCallback(Window, window_refresh_callback);

    /**
     *  Set the window icon.
     */
    //GLFWimage images[2];
    //images[0] = load_icon("my_icon.png");
    //images[1] = load_icon("my_icon_small.png");
    //glfwSetWindowIcon(Window, 2, images);

    /**
     *
     */
    MainWindow = glfwGetWin32Window(Window);
    ShowCommand = TRUE;

    return true;
}


bool OpenGLVideoDriver::Toggle_Fullscreen()
{
    Windowed = !Windowed;

    return Create_Window_Internal();
}


void OpenGLVideoDriver::Close_Window(bool force)
{
    glfwSetWindowShouldClose(Window, GL_TRUE);
}


void OpenGLVideoDriver::Destroy_Window(bool force)
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}


// TODO
void OpenGLVideoDriver::Show_Window()
{
    glfwShowWindow(Window);
}


// TODO
void OpenGLVideoDriver::Hide_Window()
{
    glfwHideWindow(Window);
}


// TODO
void OpenGLVideoDriver::Minimize_Window()
{
    glfwIconifyWindow(Window);
}


void OpenGLVideoDriver::Maximize_Window()
{
    if (Windowed) {
        glfwMaximizeWindow(Window);
    } else {
        glfwRestoreWindow(Window);
    }
}


bool OpenGLVideoDriver::Create_Window_Internal()
{
    GLFWmonitor *monitor = nullptr;

    glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    int width = WindowWidth;
    int height = WindowHeight;

    if (!Windowed) {

        /**
         *  Get the current Desktop screen resolution and colour depth.
         */
        const GLFWvidmode *video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
        width = video_mode->width;
        height = video_mode->height;

        glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);

    } else {
        glfwWindowHint(GLFW_FOCUSED, GL_TRUE);

    }

    if (Borderless) {
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }

    /**
     *  Open the window at the current Desktop resolution and colour depth.
     */
    Window = glfwCreateWindow(width, height, WindowTitle.Peek_Buffer(), monitor, nullptr);
    if (!Window) {
        DEBUG_ERROR("glfwCreateWindow failed in %s!\n", __FUNCTION__);
        return false;
    }

    glfwMakeContextCurrent(Window);

    return true;
}


void OpenGLVideoDriver::Window_Size_Callback(GLFWwindow *window, int window_width, int window_height)
{
    // TODO
#if 0
    width = window_width;
    height = window_height;

    aspect = 16.0f / 9.0f;
    perspective = glm::perspective(glm::radians(FOV), aspect, P1, P2);
#endif
}


void OpenGLVideoDriver::Window_Iconify_Callback(GLFWwindow *window, int iconified)
{
    if (iconified) {
        // The window was iconified.
    } else {
        // The window was restored.
    }
}


void OpenGLVideoDriver::Window_Maximize_Callback(GLFWwindow *window, int maximized)
{
    if (maximized) {
        // The window was maximized.
    } else {
        // The window was restored.
    }
}


void OpenGLVideoDriver::Window_Focus_Callback(GLFWwindow *window, int focused)
{
    if (focused) {
        // The window gained input focus.
    } else {
        // The window lost input focus.
    }
}


void OpenGLVideoDriver::Window_Refresh_Callback(GLFWwindow *window)
{
    // TODO

    glfwSwapBuffers(window);
}
