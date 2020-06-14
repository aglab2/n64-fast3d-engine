#include "plugin.h"

const char* Plugin::sName()
{
    return "LINK's Fast3DEngine";
}

void Plugin::sSetInfo(GFX_INFO i)
{
    gfxInfo_ = i;
    GetWindowRect(gfxInfo_.hStatusBar, &statusRect_);
}

GFX_INFO& Plugin::sInfo()
{
    return gfxInfo_;
}

HWND Plugin::sHWnd()
{
    return gfxInfo_.hWnd;
}

int Plugin::sStatusBarHeight()
{
    return statusRect_.bottom - statusRect_.top - 1;
}

void Plugin::sResize()
{
    int width = config_.width();
    int height = config_.height();
    RECT windowRect;

    GetClientRect(gfxInfo_.hWnd, &windowRect);

    auto offset = sStatusBarHeight();
    windowRect.right = windowRect.left + width - 1;
    windowRect.bottom = windowRect.top + height - 1 + offset;

    AdjustWindowRect(&windowRect, GetWindowLong(sHWnd(), GWL_STYLE), GetMenu(sHWnd()) != NULL);

    SetWindowPos(sHWnd(), NULL, 0, 0, windowRect.right - windowRect.left + 1,
        windowRect.bottom - windowRect.top + 2, SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOMOVE);
}

GFX_INFO* plugin_gfx_info()
{
    return &Plugin::info();
}
