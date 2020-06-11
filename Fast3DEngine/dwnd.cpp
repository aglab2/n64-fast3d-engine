#include "dwnd.h"
#include <Windows.h>

#include "Gfx #1.3.h"
#include <GL/glew.h>

/*
#define WIDTH 1280
#define HEIGHT 960
*/
#define WIDTH 800
#define HEIGHT 600

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

static void resizeWindow()
{
    RECT windowRect, statusRect, toolRect;

    auto width = WIDTH;
    auto height = HEIGHT;

    GetClientRect(GfxInfo.hWnd, &windowRect);
    GetWindowRect(GfxInfo.hStatusBar, &statusRect);

    toolRect.bottom = toolRect.top = 0;

    auto offset = (statusRect.bottom - statusRect.top);
    windowRect.right = windowRect.left + width - 1;
    windowRect.bottom = windowRect.top + height - 1 + offset;

    AdjustWindowRect(&windowRect, GetWindowLong(GfxInfo.hWnd, GWL_STYLE), GetMenu(GfxInfo.hWnd) != NULL);

    SetWindowPos(GfxInfo.hWnd, NULL, 0, 0, windowRect.right - windowRect.left + 1,
        windowRect.bottom - windowRect.top + 1 + toolRect.bottom - toolRect.top + 1, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

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

    /*
    PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB =
        (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");

    if (wglGetExtensionsStringARB != NULL) {
        const char* wglextensions = wglGetExtensionsStringARB(hDC);

        if (strstr(wglextensions, "WGL_ARB_create_context_profile") != nullptr) {
            PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB =
                (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

            GLint majorVersion = 0;
            glGetIntegerv(GL_MAJOR_VERSION, &majorVersion);
            GLint minorVersion = 0;
            glGetIntegerv(GL_MINOR_VERSION, &minorVersion);

            const int attribList[] =
            {
                WGL_CONTEXT_MAJOR_VERSION_ARB, majorVersion,
                WGL_CONTEXT_MINOR_VERSION_ARB, minorVersion,
                WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
                0        //End
            };

            HGLRC coreHrc = wglCreateContextAttribsARB(hDC, 0, attribList);
            if (coreHrc != NULL) {
                wglDeleteContext(hRC);
                wglMakeCurrent(hDC, coreHrc);
                hRC = coreHrc;
            }
        }

        if (strstr(wglextensions, "WGL_EXT_swap_control") != nullptr) {
            PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
            wglSwapIntervalEXT(0);
        }
    }
    */

    resizeWindow();
}

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
    *width = WIDTH;
    *height = HEIGHT;
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
