#pragma once

#include <stdint.h>

#include "gbi.h"

#define MAX_BUFFERED 256
#define MAX_LIGHTS 2
#define MAX_VERTICES 64

extern struct GfxDimensions gfx_current_dimensions;
extern struct GfxWindowManagerAPI* gfx_wapi;
extern struct GfxRenderingAPI* gfx_rapi;
extern uint32_t gSegments[16];

struct RGBA {
    uint8_t r, g, b, a;
};

struct XYWidthHeight {
    uint16_t x, y, width, height;
};

struct LoadedVertex {
    float x, y, z, w;
    float u, v;
    struct RGBA color;
    uint8_t clip_rej;
};

struct TextureHashmapNode {
    struct TextureHashmapNode* next;

    const uint8_t* texture_addr;
    uint8_t fmt, siz;

    uint64_t hash;

    uint32_t texture_id;
    uint8_t cms, cmt;
    bool linear_filter;
};
extern struct GfxTextureCache {
    struct TextureHashmapNode* hashmap[1024];
    struct TextureHashmapNode pool[512];
    uint32_t pool_pos;
} gfx_texture_cache;

struct ColorCombiner {
    uint32_t cc_id;
    struct ShaderProgram* prg;
    uint8_t shader_input_mapping[2][4];
};

extern struct ColorCombiner color_combiner_pool[64];
extern uint8_t color_combiner_pool_size;

extern struct RSP {
    float modelview_matrix_stack[11][4][4];
    uint8_t modelview_matrix_stack_size;

    float MP_matrix[4][4];
    float P_matrix[4][4];

    Light_t current_lights[MAX_LIGHTS + 1];
    float current_lights_coeffs[MAX_LIGHTS][3];
    float current_lookat_coeffs[2][3]; // lookat_x, lookat_y
    uint8_t current_num_lights; // includes ambient light
    bool lights_changed;

    uint32_t geometry_mode;
    int16_t fog_mul, fog_offset;

    struct {
        // U0.16
        uint16_t s, t;
    } texture_scaling_factor;

    struct LoadedVertex loaded_vertices[MAX_VERTICES + 4];
} rsp;

extern struct RDP {
    const uint8_t* palette;
    struct {
        const uint8_t* addr;
        uint8_t siz;
        uint8_t tile_number;
    } texture_to_load;
    struct {
        const uint8_t* addr;
        uint32_t size_bytes;
    } loaded_texture[2];
    struct {
        uint8_t fmt;
        uint8_t siz;
        uint8_t cms, cmt;
        uint16_t uls, ult, lrs, lrt; // U10.2
        uint32_t line_size_bytes;
    } texture_tile;
    bool textures_changed[2];

    uint32_t other_mode_l, other_mode_h;
    uint32_t combine_mode;

    struct RGBA env_color, prim_color, fog_color, fill_color;
    struct XYWidthHeight viewport, scissor;
    bool viewport_or_scissor_changed;
    void* z_buf_address;
    void* color_image_address;
} rdp;

extern struct RenderingState {
    bool depth_test;
    bool depth_mask;
    bool decal_mode;
    bool alpha_blend;
    struct XYWidthHeight viewport, scissor;
    struct ShaderProgram* shader_program;
    struct TextureHashmapNode* textures[2];
} rendering_state;

extern float buf_vbo[MAX_BUFFERED * (26 * 3)]; // 3 vertices in a triangle and 26 floats per vtx
extern size_t buf_vbo_len;
extern size_t buf_vbo_num_tris;

enum class InstructionProcessResult
{
    STOP,
    CONTINUE,
};

void gfx_inc(Gfx** gfx, uintptr_t* segAddr);
void gfx_dec(Gfx** gfx, uintptr_t* segAddr);
void gfx_run_dl(Gfx* cmd, uintptr_t segAddr);

void gfx_texture_cache_drop();
void gfx_color_combiner_cache_drop();
void gfx_sp_reset();
void gfx_flush(void);

enum class UCodes
{
    F3D,
    F3DEX,
    F3DEX2,
};

extern UCodes gUCode;

UCodes parseUcode(uint32_t ucStart, uint32_t ucDStart, uint32_t ucDSize);