#pragma once

#include <Windows.h>
#include "Gfx #1.3.h"

#ifdef __cplusplus

class Plugin
{
public:
    static const char* name()        { return me().sName(); }
    static void setInfo(GFX_INFO i)  { return me().sSetInfo(i); }
    static GFX_INFO& info()          { return me().sInfo(); }
    static HWND hWnd()               { return me().sHWnd(); }
    static int statusBarHeight()     { return me().sStatusBarHeight(); }

    static void resize(int w, int h) { return me().sResize(w, h); }

private:
    const char* sName();
    void sSetInfo(GFX_INFO);
    GFX_INFO& sInfo();
    HWND sHWnd();
    int sStatusBarHeight();

    void sResize(int w, int h);

    static Plugin& me()
    {
        static Plugin plugin;
        return plugin;
    }

    GFX_INFO gfxInfo_;
    RECT statusRect_;
};

extern "C"
{
#endif

GFX_INFO* plugin_gfx_info();
int plugin_gfx_status_bar_height();

#ifdef __cplusplus
}
#endif