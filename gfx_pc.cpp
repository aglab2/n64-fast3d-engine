#include "gfx_api.h"
#include "gfx_pc.h"

#include "gfx_rendering_api.h"
#include "gfx_window_manager_api.h"

#include <string.h>

static bool dropped_frame;

static uint32_t gUCStart = -1;
static uint32_t gUCDStart = -1;
static uint32_t gUCDSize = -1;

void gfx_load_ucode(uint32_t ucStart, uint32_t ucDStart, uint32_t ucDSize)
{
    if (gUCStart == ucStart)
        return;

    gUCStart = ucStart;
    gUCDStart = ucDStart;
    gUCDSize = ucDSize;

    gUCode = parseUcode(ucStart, ucDStart, ucDSize);
}

void gfx_get_dimensions(uint32_t* width, uint32_t* height) {
    gfx_wapi->get_dimensions(width, height);
}

void gfx_init(struct GfxWindowManagerAPI* wapi, struct GfxRenderingAPI* rapi, const char* game_name, bool start_in_fullscreen) {
    gfx_wapi = wapi;
    gfx_rapi = rapi;

    gfx_wapi->init(game_name, start_in_fullscreen);
    gfx_rapi->init();
}

void gfx_deinit(void)
{
    if (gfx_rapi == NULL || gfx_wapi == NULL)
        return;

    gfx_texture_cache_drop();
    gfx_color_combiner_cache_drop();
    memset(&rdp, 0, sizeof(rdp));
    memset(&rsp, 0, sizeof(rsp));
    memset(&rendering_state, 0, sizeof(rendering_state));

    gfx_rapi->deinit();
    gfx_wapi->deinit();
    gfx_rapi = NULL;
    gfx_wapi = NULL;
}

struct GfxRenderingAPI* gfx_get_current_rendering_api(void) {
    return gfx_rapi;
}

struct GfxWindowManagerAPI* gfx_get_current_window_manager_api(void) {
    return gfx_wapi;
}

void gfx_start_frame(void) {
    for (int i = 0; i < 16; i++)
        gSegments[i] = 0;

    gfx_wapi->get_dimensions(&gfx_current_dimensions.width, &gfx_current_dimensions.height);
    if (gfx_current_dimensions.height == 0) {
        // Avoid division by zero
        gfx_current_dimensions.height = 1;
    }
    gfx_current_dimensions.fixup_factor = (4.0f / 3.0f) / ((float)gfx_current_dimensions.width / (float)gfx_current_dimensions.height);
}

void gfx_run(void* commands) {
    gfx_sp_reset();

    if (!gfx_wapi->start_frame()) {
        dropped_frame = true;
        return;
    }
    dropped_frame = false;

    if (!gfx_wapi->start_frame()) {
        dropped_frame = true;
        return;
    }
    dropped_frame = false;

    gfx_rapi->start_frame();
    gfx_run_dl((Gfx*)commands, (uintptr_t)commands);
    gfx_flush();
}

void gfx_end_frame(void) {
    if (!dropped_frame) {
        gfx_rapi->end_frame();
        gfx_wapi->swap_buffers_begin();
        gfx_rapi->finish_render();
        gfx_wapi->swap_buffers_end();
    }
}
