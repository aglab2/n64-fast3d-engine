#include "dwnd.h"
#include "plugin.h"

#include <GL/glew.h>
#include <GL/wglew.h>

#include "../gfx_screen_config.h"

#include "plugin.h"

static HGLRC hRC;
static HDC   hDC;

void dwnd_init(const char* game_name, bool start_in_fullscreen)
{
    auto pluginNameW = Plugin::name();
    auto& gfxInfo = Plugin::info();

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


    if (gfxInfo.hWnd == NULL)
        gfxInfo.hWnd = GetActiveWindow();

    if ((hDC = GetDC(gfxInfo.hWnd)) == NULL) {
        MessageBox(gfxInfo.hWnd, "Error while getting a device context!", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    int pixelFormat;
    if ((pixelFormat = ChoosePixelFormat(hDC, &pfd)) == 0) {
        MessageBox(gfxInfo.hWnd, "Unable to find a suitable pixel format!", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    if ((SetPixelFormat(hDC, pixelFormat, &pfd)) == FALSE) {
        MessageBox(gfxInfo.hWnd, "Error while setting pixel format!", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    if ((hRC = wglCreateContext(hDC)) == NULL) {
        MessageBox(gfxInfo.hWnd, "Error while creating OpenGL context!", pluginNameW, MB_ICONERROR | MB_OK);
        RomClosed();
        return;
    }

    if (!wglMakeCurrent(hDC, hRC)) {
        MessageBox(gfxInfo.hWnd, "Failed to activate OpenGL 3.3 rendering context.", pluginNameW, MB_ICONERROR | MB_OK);
        return;
    }

    glewInit();
    wglewInit();

    Plugin::resize(false);
}

void dwnd_deinit(void)
{
    auto& gfxInfo = Plugin::info();

    wglMakeCurrent(NULL, NULL);

    if (hRC != NULL) {
        wglDeleteContext(hRC);
        hRC = NULL;
    }

    if (hDC != NULL) {
        ReleaseDC(gfxInfo.hWnd, hDC);
        hDC = NULL;
    }
}

void dwnd_set_fullscreen_changed_callback(void (*on_fullscreen_changed)(bool is_now_fullscreen))
{

}

void dwnd_set_fullscreen(bool enable) {

}

void dwnd_get_dimensions(uint32_t* width, uint32_t* height)
{
    *width = Plugin::config().width();
    *height = Plugin::config().height();
}

void dwnd_swap_buffers_begin(void)
{
    if (hDC == NULL)
        SwapBuffers(wglGetCurrentDC());
    else
        SwapBuffers(hDC);
}

double dwnd_get_time(void)
{
    return 0.;
}

struct GfxWindowManagerAPI gfx_dwnd = 
{
    dwnd_init,
    dwnd_deinit,
    dwnd_set_fullscreen_changed_callback,
    dwnd_set_fullscreen,
    dwnd_get_dimensions,
    []() { return true; }, /* start_frame */
    dwnd_swap_buffers_begin,
    []() { } /* dwnd_swap_buffers_end */,
    dwnd_get_time,
};
