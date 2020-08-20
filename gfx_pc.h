#ifndef GFX_PC_H
#define GFX_PC_H

#include <stdbool.h>
#include <stdint.h>

struct GfxRenderingAPI;
struct GfxWindowManagerAPI;

struct GfxDimensions {
    uint32_t width, height;
    float fixup_factor;
};

extern struct GfxDimensions gfx_current_dimensions;

#ifdef __cplusplus
extern "C" {
#endif

void gfx_init(struct GfxWindowManagerAPI *wapi, struct GfxRenderingAPI *rapi, const char *game_name, bool start_in_fullscreen);
struct GfxRenderingAPI *gfx_get_current_rendering_api(void);
void gfx_deinit(void);
void gfx_start_frame(void);
void gfx_run(void* commands);
void gfx_end_frame(void);

struct GfxRenderingAPI* gfx_get_current_rendering_api(void);
struct GfxWindowManagerAPI* gfx_get_current_window_manager_api(void);

#ifdef __cplusplus
}
#endif

#endif
