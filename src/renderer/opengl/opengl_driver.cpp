/*******************************************************************************
/*                 O P E N  S O U R C E  --  V I N I F E R A                  **
/*******************************************************************************
 *
 *  @project       Vinifera
 *
 *  @file          OPENGL_RENDERER.CPP
 *
 *  @author        CCHyper
 *
 *  @brief         
 *
 *  @license       Vinifera is free software: you can redistribute it and/or
 *                 modify it under the terms of the GNU General Public License
 *                 as published by the Free Software Foundation, either version
 *                 3 of the License, or (at your option) any later version.
 *
 *                 Vinifera is distributed in the hope that it will be
 *                 useful, but WITHOUT ANY WARRANTY; without even the implied
 *                 warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *                 PURPOSE. See the GNU General Public License for more details.
 *
 *                 You should have received a copy of the GNU General Public
 *                 License along with this program.
 *                 If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#if defined(OPENGL_RENDERER)

#include "opengl_driver.h"
#include "tibsun_globals.h"
#include "debughandler.h"
#include "asserthandler.h"

#if defined(OPENGL_GLAD)
#include <glad/glad.h>
#elif defined(OPENGL_GLEW)
#include <gl/glew.h>
#endif

#define GLFW_EXPOSE_NATIVE_WGL
#define GLFW_EXPOSE_NATIVE_WIN32

#include <glfw/glfw3.h>
#include <glfw/glfw3native.h>



 /**
  *  x
  *
  *  @author: CCHyper
  */
OpenGLVideoDriver::OpenGLVideoDriver() :
    VideoDriver("OpenGL"),
    Window(nullptr)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
OpenGLVideoDriver::~OpenGLVideoDriver()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool OpenGLVideoDriver::Toggle_Fullscreen()
{
    IsWindowed = !IsWindowed;

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Focus_Loss()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Focus_Restore()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Set_Cursor_Clip()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Clear_Cursor_Clip()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool OpenGLVideoDriver::Update_Screen(XSurface *surface, Rect *src_rect, Rect *dest_rect)
{
    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool OpenGLVideoDriver::Clear_Screen(bool present)
{
    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool OpenGLVideoDriver::Set_Video_Mode(HWND hWnd, int width, int height, int bits_per_pixel)
{
    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Reset_Video_Mode()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Release(HWND hWnd)
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool OpenGLVideoDriver::Prep_Renderer(HWND hWnd)
{
    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Check_Overlapped_Blit_Capability()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Wait_Blit()
{
}


/**
 *  x
 *
 *  @author: CCHyper
 */
extern void __cdecl Write_Surface_Data_To_File(const char *filename, XSurface *surface);
bool OpenGLVideoDriver::Flip(XSurface *surface)
{
    // TEST
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(Window);


#if 0
    static int _counter = 0;
    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "OGL_%04d.png", _counter++);
    Write_Surface_Data_To_File(buffer, surface);
#endif

    return true;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool OpenGLVideoDriver::Frame_Limiter(bool force_blit)
{
    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Shutdown()
{
    // Terminates GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Close_Window(bool force)
{
    glfwSetWindowShouldClose(Window, GL_TRUE);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Destroy_Window(bool force)
{
    glfwDestroyWindow(Window);
    glfwTerminate();
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Show_Window()
{
    glfwShowWindow(Window);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Hide_Window()
{
    glfwHideWindow(Window);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Minimize_Window()
{
    glfwIconifyWindow(Window);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Maximize_Window()
{
    if (IsWindowed) {
        glfwMaximizeWindow(Window);
    } else {
        glfwRestoreWindow(Window);
    }
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool OpenGLVideoDriver::Internal_Create_Primary(Surface *primary_surface, XSurface **backbuffer_surface)
{
    return false;
}


/**
 *  x
 *
 *  @author: CCHyper
 */
bool OpenGLVideoDriver::Internal_Create_Window(HINSTANCE hInstance)
{
    /**
     *  Init GLFW.
     */
    if (glfwInit() == 0) {
        DEBUG_ERROR("OpenGL - glfwInit failed in %s!\n", __FUNCTION__);
        return false;
    }

    /**
     *  Set all the required options for GLFW.
     */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    int width = WindowWidth;
    int height = WindowHeight;

    /**
     *  x
     */
    int monitor_count;
    GLFWmonitor **monitors = glfwGetMonitors(&monitor_count);
    DEBUG_INFO("OpenGL - Monitor count = %d\n", monitor_count);

    GLFWmonitor *monitor = nullptr;
    if (MonitorToUse > -1 && MonitorToUse <= monitor_count) {
        DEBUG_INFO("OpenGL - Using monitor %d\n", MonitorToUse);
        monitor = monitors[MonitorToUse];
    } else {
        DEBUG_INFO("OpenGL - Using primary monitor.\n");
        monitor = glfwGetPrimaryMonitor();
    }

    /**
     *  Get the current Desktop screen resolution and colour depth.
     */
    const GLFWvidmode *video_mode = glfwGetVideoMode(monitor);
    int display_width = video_mode->width;
    int display_height = video_mode->height;

    DEBUG_INFO("OpenGL - Video mode: %d x %d.\n", display_width, display_height);

    if (IsWindowed) {
        glfwWindowHint(GLFW_FOCUSED, GL_TRUE);

    } else {

        glfwWindowHint(GLFW_RED_BITS, video_mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, video_mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, video_mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, video_mode->refreshRate);

        DEBUG_INFO("OpenGL - Video info: redBits %d, greenBits %d, blueBits %d, refreshRate %d.\n",
            video_mode->redBits, video_mode->greenBits, video_mode->blueBits, video_mode->refreshRate);

        width = display_width;
        height = display_height;
    }

    if (IsBorderlessWindow) {
        DEBUG_INFO("OpenGL - Borderless window.\n");
        glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
    }

    DEBUG_INFO("OpenGL - Window size: %d x %d.\n", width, height);

    /**
     *  Open the window at the current Desktop resolution and colour depth.
     */
    Window = glfwCreateWindow(width, height, WindowTitle.Peek_Buffer(), monitor, nullptr);
    if (!Window) {
        DEBUG_ERROR("glfwCreateWindow failed in %s!\n", __FUNCTION__);
        return false;
    }

    /**
     *  Fetch the desktop size, calculate the screen center position the window and move it.
     */
    if (IsWindowed) {

        RECT workarea;
        SystemParametersInfo(SPI_GETWORKAREA, 0, &workarea, 0);

        RECT rect;
        SetRect(&rect, 0, 0, WindowWidth, WindowHeight);

        int x_pos = (display_width - WindowWidth) / 2;
        int y_pos = (((display_height - WindowHeight) / 2) - (display_height - workarea.bottom));

        DEBUG_INFO("OpenGL - Moving window (%d,%d,%d,%d).\n",
            x_pos, y_pos, (rect.right - rect.left), (rect.bottom - rect.top));

        glfwSetWindowPos(Window, x_pos, y_pos);
    }

    glfwMakeContextCurrent(Window);

#if defined(OPENGL_GLAD)
    /**
     *  Load the OpenGL function pointers.
     */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        DEBUG_ERROR("OpenGL - gladLoadGLLoader failed in %s!\n", __FUNCTION__);
        return false;
    }
#elif defined(OPENGL_GLEW)
    if (glewInit() != GLEW_OK) {
        DEBUG_ERROR("OpenGL - glewInit failed in %s!\n", __FUNCTION__);
        return false;
    }
#endif

    int glfw_ver_major = glfwGetWindowAttrib(Window, GLFW_CONTEXT_VERSION_MAJOR);
    int glfw_ver_minor = glfwGetWindowAttrib(Window, GLFW_CONTEXT_VERSION_MINOR);
    int glfw_ver_rev = glfwGetWindowAttrib(Window, GLFW_CONTEXT_REVISION);

    DEBUG_INFO("OpenGL - Major:%d Minor:%d Rev:%d\n", glfw_ver_major, glfw_ver_minor, glfw_ver_rev);

    /**
     *  Set the window callbacks.
     */
    //glfwSetWindowSizeCallback(Window, Window_Size_Callback);
    //glfwSetWindowIconifyCallback(Window, Window_Iconify_Callback);
    //glfwSetWindowMaximizeCallback(Window, Window_Maximize_Callback);
    //glfwSetWindowFocusCallback(Window, Window_Focus_Callback);
    //glfwSetWindowRefreshCallback(Window, Window_Key_Callback);
    //glfwSetKeyCallback(Window, Window_Key_Callback);

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

    /**
     *  Define the viewport dimensions.
     */
    glViewport(0, 0, width, height);

    return true;
}

// Callback for when the window is resized.
void OpenGLVideoDriver::Window_Size_Callback(GLFWwindow *window, int width, int height)
{
#if 0
    width = window_width;
    height = window_height;

    aspect = 16.0f / 9.0f;
    perspective = glm::perspective(glm::radians(FOV), aspect, P1, P2);
#endif

    glViewport(0, 0, width, height);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Window_Iconify_Callback(GLFWwindow *window, int iconified)
{
    if (iconified) {
        // The window was iconified.
    } else {
        // The window was restored.
    }
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Window_Maximize_Callback(GLFWwindow *window, int maximized)
{
    if (maximized) {
        // The window was maximized.
    } else {
        // The window was restored.
    }
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Window_Focus_Callback(GLFWwindow *window, int focused)
{
    if (focused) {
        // The window gained input focus.
    } else {
        // The window lost input focus.
    }
}


/**
 *  x
 *
 *  @author: CCHyper
 */
void OpenGLVideoDriver::Window_Refresh_Callback(GLFWwindow *window)
{
    glfwSwapBuffers(window);
}


/**
 *  x
 *
 *  @author: CCHyper
 */
// Callback for when a key is pressed.
void OpenGLVideoDriver::Window_Key_Callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == (GLFW_KEY_LEFT_ALT|GLFW_KEY_F4) && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

#endif
