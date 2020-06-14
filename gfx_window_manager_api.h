#ifndef GFX_WINDOW_MANAGER_API_H
#define GFX_WINDOW_MANAGER_API_H

#include <stdint.h>
#include <stdbool.h>

struct GfxWindowManagerAPI {
    void (*init)(const char *game_name, bool start_in_fullscreen);
    void (*deinit)(void);
    void (*set_fullscreen_changed_callback)(void (*on_fullscreen_changed)(bool is_now_fullscreen));
    void (*set_fullscreen)(bool enable);
    void (*get_dimensions)(uint32_t *width, uint32_t *height);
    bool (*start_frame)(void);
    void (*swap_buffers_begin)(void);
    void (*swap_buffers_end)(void);
    double (*get_time)(void); // For debug
};

#endif
