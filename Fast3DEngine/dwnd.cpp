#include "dwnd.h"
#include <Windows.h>

#include "Gfx #1.3.h"
#include <GL/glew.h>
#include <GL/wglew.h>

#include "../gfx_screen_config.h"

static char pluginNameW[] = "LINK's Renderer";

HGLRC	hRC;
HDC		hDC;

extern "C" GFX_INFO GfxInfo;

typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGARBPROC) (HDC hdc);
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC) (HDC hDC, HGLRC hShareContext, const int* attribList);
typedef BOOL(WINAPI* PFNWGLSWAPINTERVALEXTPROC) (int interval);

#define WGL_CONTEXT_MAJOR_VERSION_ARB     0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB     0x2092

#define WGL_CONTEXT_PROFILE_MASK_ARB      0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB  0x00000001

extern "C" RECT gStatusRect;
RECT gStatusRect;

static void resizeWindow()
{
    RECT windowRect;

    auto width = DESIRED_SCREEN_WIDTH;
    auto height = DESIRED_SCREEN_HEIGHT;

    GetClientRect(GfxInfo.hWnd, &windowRect);
    GetWindowRect(GfxInfo.hStatusBar, &gStatusRect);

    auto offset = (gStatusRect.bottom - gStatusRect.top);
    windowRect.right = windowRect.left + width - 1;
    windowRect.bottom = windowRect.top + height - 1 + offset;

    AdjustWindowRect(&windowRect, GetWindowLong(GfxInfo.hWnd, GWL_STYLE), GetMenu(GfxInfo.hWnd) != NULL);

    SetWindowPos(GfxInfo.hWnd, NULL, 0, 0, windowRect.right - windowRect.left + 1,
        windowRect.bottom - windowRect.top + 2, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

static bool initExtensions = false;

static void
init_opengl_extensions(void)
{
    if (initExtensions)
        return;

    initExtensions = true;

    // Before we can load extensions, we need a dummy OpenGL context, created using a dummy window.
    // We use a dummy window because you can only set the pixel format for a window once. For the
    // real window, we want to use wglChoosePixelFormatARB (so we can potentially specify options
    // that aren't available in PIXELFORMATDESCRIPTOR), but we can't load and use that before we
    // have a context.
    WNDCLASSA window_class = {
         CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
         DefWindowProcA,
         0, 0,
         GetModuleHandle(0),
         nullptr, nullptr, nullptr, nullptr, "Dummy_WGL_djuasiodwa",
    };

    if (!RegisterClassA(&window_class)) {
        MessageBox(GfxInfo.hWnd, "Failed to register dummy OpenGL window!", pluginNameW, MB_ICONERROR | MB_OK);
    }

    HWND dummy_window = CreateWindowExA(
        0,
        window_class.lpszClassName,
        "Dummy OpenGL Window",
        0,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        0,
        0,
        window_class.hInstance,
        0);

    if (!dummy_window) {
        MessageBox(GfxInfo.hWnd, "Failed to create dummy window!", pluginNameW, MB_ICONERROR | MB_OK);
    }

    HDC dummy_dc = GetDC(dummy_window);

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd
        1,                                // version number
        PFD_DRAW_TO_WINDOW |              // support window
        PFD_SUPPORT_OPENGL |              // support OpenGL
        PFD_DOUBLEBUFFER,                 // double buffered
        PFD_TYPE_RGBA,                    // RGBA type
        32,								  // color depth
        0, 0, 0, 0, 0, 0,                 // color bits ignored
        0,                                // no alpha buffer
        0,                                // shift bit ignored
        0,                                // no accumulation buffer
        0, 0, 0, 0,                       // accum bits ignored
        32,								  // z-buffer
        0,                                // no stencil buffer
        0,                                // no auxiliary buffer
        PFD_MAIN_PLANE,                   // main layer
        0,                                // reserved
        0, 0, 0                           // layer masks ignored
    };

    int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
    if (!pixel_format) {
        MessageBox(GfxInfo.hWnd, "Unable to find a suitable pixel format!", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }
    if (!SetPixelFormat(dummy_dc, pixel_format, &pfd)) {
        MessageBox(GfxInfo.hWnd, "Error while setting pixel format!", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    HGLRC dummy_context = wglCreateContext(dummy_dc);
    if (!dummy_context) {
        MessageBox(GfxInfo.hWnd, "Error while creating OpenGL context!", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    if (!wglMakeCurrent(dummy_dc, dummy_context)) {
        MessageBox(GfxInfo.hWnd, "Error while making OpenGL context current!", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    glewInit();
    wglewInit();

    wglMakeCurrent(dummy_dc, 0);
    wglDeleteContext(dummy_context);
    ReleaseDC(dummy_window, dummy_dc);
    DestroyWindow(dummy_window);
}


void dwnd_init(const char* game_name)
{
    init_opengl_extensions();

    // Now we can choose a pixel format the modern way, using wglChoosePixelFormatARB.
    int pixel_format_attribs[] = 
    {
        WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
        WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
        WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
        WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
        WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
        WGL_COLOR_BITS_ARB,         32,
        WGL_DEPTH_BITS_ARB,         24,
        WGL_STENCIL_BITS_ARB,       0,
        WGL_SAMPLE_BUFFERS_ARB, 1, // Number of buffers (must be 1 at time of writing)
        WGL_SAMPLES_ARB, 4,        // Number of samples
        0
    };

    if (GfxInfo.hWnd == NULL)
        GfxInfo.hWnd = GetActiveWindow();

    if ((hDC = GetDC(GfxInfo.hWnd)) == NULL) {
        MessageBox(GfxInfo.hWnd, "Error while getting a device context!", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    int pixel_format;
    UINT num_formats;
    wglChoosePixelFormatARB(hDC, pixel_format_attribs, 0, 1, &pixel_format, &num_formats);
    if (!num_formats) {
        MessageBox(GfxInfo.hWnd, "Failed to set the OpenGL 3.3 pixel format.", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    PIXELFORMATDESCRIPTOR pfd;
    DescribePixelFormat(hDC, pixel_format, sizeof(pfd), &pfd);
    if (!SetPixelFormat(hDC, pixel_format, &pfd)) {
        MessageBox(GfxInfo.hWnd, "Failed to set the OpenGL 3.3 pixel format.", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    if ((hRC = wglCreateContext(hDC)) == NULL) {
        MessageBox(GfxInfo.hWnd, "Error while creating OpenGL context!", pluginNameW, MB_ICONERROR | MB_OK);
        RomClosed();
        return;
    }

    if (!wglMakeCurrent(hDC, hRC)) {
        MessageBox(GfxInfo.hWnd, "Failed to activate OpenGL 3.3 rendering context.", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    resizeWindow();
}

/*
void dwnd_init(const char* game_name)
{
    int pixelFormat;

    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),    // size of this pfd
        1,                                // version number
        PFD_DRAW_TO_WINDOW |              // support window
        PFD_SUPPORT_OPENGL |              // support OpenGL
        PFD_DOUBLEBUFFER,                 // double buffered
        PFD_TYPE_RGBA,                    // RGBA type
        32,								  // color depth
        0, 0, 0, 0, 0, 0,                 // color bits ignored
        0,                                // no alpha buffer
        0,                                // shift bit ignored
        0,                                // no accumulation buffer
        0, 0, 0, 0,                       // accum bits ignored
        32,								  // z-buffer
        0,                                // no stencil buffer
        0,                                // no auxiliary buffer
        PFD_MAIN_PLANE,                   // main layer
        0,                                // reserved
        0, 0, 0                           // layer masks ignored
    };

    if (GfxInfo.hWnd == NULL)
        GfxInfo.hWnd = GetActiveWindow();

    if ((hDC = GetDC(GfxInfo.hWnd)) == NULL) {
        MessageBox(GfxInfo.hWnd, "Error while getting a device context!", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    if ((pixelFormat = ChoosePixelFormat(hDC, &pfd)) == 0) {
        MessageBox(GfxInfo.hWnd, "Unable to find a suitable pixel format!", pluginNameW, MB_ICONERROR | MB_OK);
        RomClosed();
        return;
    }

    if ((SetPixelFormat(hDC, pixelFormat, &pfd)) == FALSE) {
        MessageBox(GfxInfo.hWnd, "Error while setting pixel format!", pluginNameW, MB_ICONERROR | MB_OK);
        RomClosed();
        return;
    }

    if ((hRC = wglCreateContext(hDC)) == NULL) {
        MessageBox(GfxInfo.hWnd, "Error while creating OpenGL context!", pluginNameW, MB_ICONERROR | MB_OK);
        RomClosed();
        return;
    }

    if ((wglMakeCurrent(hDC, hRC)) == FALSE) {
        MessageBox(GfxInfo.hWnd, "Error while making OpenGL context current!", pluginNameW, MB_ICONERROR | MB_OK);
        RomClosed();
        return;
    }

    glewInit();

    PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB =
        (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

    if (wglGetExtensionsStringARB != NULL) {
        const char* wglextensions = wglGetExtensionsStringARB(hDC);
        if (strstr(wglextensions, "WGL_EXT_swap_control") != nullptr) {
            PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
            wglSwapIntervalEXT(0);
        }
    }

    resizeWindow();
}
*/

void dwnd_deinit(void)
{
    wglMakeCurrent(NULL, NULL);

    if (hRC != NULL) {
        wglDeleteContext(hRC);
        hRC = NULL;
    }

    if (hDC != NULL) {
        ReleaseDC(GfxInfo.hWnd, hDC);
        hDC = NULL;
    }
}

void dwnd_set_keyboard_callbacks(bool (*on_key_down)(int scancode), bool (*on_key_up)(int scancode), void (*on_all_keys_up)(void))
{

}

void dwnd_main_loop(void (*run_one_game_iter)(void))
{

}

void dwnd_get_dimensions(uint32_t* width, uint32_t* height)
{
    *width = DESIRED_SCREEN_WIDTH;
    *height = DESIRED_SCREEN_HEIGHT;
}

void dwnd_handle_events(void)
{

}

bool dwnd_start_frame(void)
{
    return true;
}

void dwnd_swap_buffers_begin(void)
{
    if (hDC == NULL)
        SwapBuffers(wglGetCurrentDC());
    else
        SwapBuffers(hDC);
}

void dwnd_swap_buffers_end(void)
{

}

double dwnd_get_time(void)
{
    return 0.;
}

struct GfxWindowManagerAPI gfx_dwnd = 
{
    dwnd_init,
    dwnd_deinit,
    dwnd_set_keyboard_callbacks,
    dwnd_main_loop,
    dwnd_get_dimensions,
    dwnd_handle_events,
    dwnd_start_frame,
    dwnd_swap_buffers_begin,
    dwnd_swap_buffers_end,
    dwnd_get_time,
};
