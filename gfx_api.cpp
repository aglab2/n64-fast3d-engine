#include "gfx_api.h"
#include "gfx_pc.h"

#include "gfx_rendering_api.h"
#include "gfx_window_manager_api.h"

#include "gfx_f3d.h"
#include "gfx_f3dex.h"
#include "gfx_f3dex2.h"

#include "Fast3DEngine/plugin.h"

#include <string.h>

UCodes gUCode;
struct GfxDimensions gfx_current_dimensions = {};
struct GfxWindowManagerAPI* gfx_wapi = nullptr;
struct GfxRenderingAPI* gfx_rapi = nullptr;
struct GfxTextureCache gfx_texture_cache = {};
struct ColorCombiner color_combiner_pool[64] = {};
uint8_t color_combiner_pool_size = {};
struct RSP rsp = {};
struct RDP rdp = {};
struct RenderingState rendering_state = {};
float buf_vbo[MAX_BUFFERED * (26 * 3)] = {};
size_t buf_vbo_len = {};
size_t buf_vbo_num_tris = {};

uint32_t gSegments[16];

void gfx_inc(Gfx** gfx, uintptr_t* segAddr)
{
    *gfx = (Gfx*)((char*)(*gfx) + 8);
    *segAddr += 8;
}

void gfx_dec(Gfx** gfx, uintptr_t* segAddr)
{
    *gfx = (Gfx*)((char*)(*gfx) - 8);
    *segAddr -= 8;
}

void gfx_run_dl(Gfx* cmd, uintptr_t segAddr)
{
    int dummy = 0;
    for (;;) {
        InstructionProcessResult res;
        switch (gUCode)
        {
        case UCodes::F3D:
            res = gfx_process_instruction_F3D(&cmd, &segAddr);
            break;

        case UCodes::F3DEX:
            res = gfx_process_instruction_F3DEX(&cmd, &segAddr);
            break;

        case UCodes::F3DEX2:
            res = gfx_process_instruction_F3DEX2(&cmd, &segAddr);
            break;

        }
        if (res == InstructionProcessResult::STOP)
            return;

        gfx_inc(&cmd, &segAddr);
    }
}

void gfx_texture_cache_drop()
{
    gfx_texture_cache.pool_pos = 0;
    for (int i = 0; i < 0x400; i++)
    {
        gfx_texture_cache.hashmap[i] = NULL;
    }
    for (int i = 0; i < sizeof(gfx_texture_cache.pool) / sizeof(struct TextureHashmapNode); i++)
    {
        gfx_rapi->delete_texture(gfx_texture_cache.pool[i].texture_id);
    }

    memset(&gfx_texture_cache, 0, sizeof(gfx_texture_cache));
}

void gfx_color_combiner_cache_drop()
{
    for (size_t i = 0; i < color_combiner_pool_size; i++) {
        gfx_rapi->delete_shader(color_combiner_pool[i].prg);
    }

    memset(color_combiner_pool, 0, sizeof(color_combiner_pool));
    color_combiner_pool_size = 0;
}

void gfx_sp_reset() {
    rsp.modelview_matrix_stack_size = 1;
    rsp.current_num_lights = 2;
    rsp.lights_changed = true;
}

void gfx_flush(void) {
    if (buf_vbo_len > 0) {
        int num = buf_vbo_num_tris;
        gfx_rapi->draw_triangles(buf_vbo, buf_vbo_len, buf_vbo_num_tris);
        buf_vbo_len = 0;
        buf_vbo_num_tris = 0;
    }
}

static void UnswapCopyWrap(const BYTE* src, uint32_t srcIdx, char* dest, uint32_t destIdx, uint32_t destMask, uint32_t numBytes)
{
    // copy leading bytes
    uint32_t leadingBytes = srcIdx & 3;
    if (leadingBytes != 0) {
        leadingBytes = 4 - leadingBytes;
        if ((uint32_t)leadingBytes > numBytes)
            leadingBytes = numBytes;
        numBytes -= leadingBytes;

        srcIdx ^= 3;
        for (uint32_t i = 0; i < leadingBytes; i++) {
            dest[destIdx & destMask] = src[srcIdx];
            ++destIdx;
            --srcIdx;
        }
        srcIdx += 5;
    }

    // copy dwords
    int numDWords = numBytes >> 2;
    while (numDWords--) {
        dest[(destIdx + 3) & destMask] = src[srcIdx++];
        dest[(destIdx + 2) & destMask] = src[srcIdx++];
        dest[(destIdx + 1) & destMask] = src[srcIdx++];
        dest[(destIdx + 0) & destMask] = src[srcIdx++];
        destIdx += 4;
    }

    // copy trailing bytes
    int trailingBytes = numBytes & 3;
    if (trailingBytes) {
        srcIdx ^= 3;
        for (int i = 0; i < trailingBytes; i++) {
            dest[destIdx & destMask] = src[srcIdx];
            ++destIdx;
            --srcIdx;
        }
    }
}

UCodes parseUcode(uint32_t ucStart, uint32_t ucDStart, uint32_t ucDSize)
{
    auto rdram = Plugin::info().RDRAM;

    char uc_data[2048];
    UnswapCopyWrap(rdram, ucDStart & 0x1FFFFFFF, uc_data, 0, 0x7FF, 2048);
	char uc_str[256];

	for (int i = 0; i < 2046; ++i) {
		if ((uc_data[i] == 'R') && (uc_data[i + 1] == 'S') && (uc_data[i + 2] == 'P')) {
			int j = 0;
			while (uc_data[i + j] > 0x0A) {
				uc_str[j] = uc_data[i + j];
				j++;
			}

			uc_str[j] = 0x00;
			if (strncmp(&uc_str[4], "SW", 2) == 0) {
				return UCodes::F3D;
			}
			else if (strncmp(&uc_str[4], "Gfx", 3) == 0) {
				if (strncmp(&uc_str[14], "F3D", 3) == 0) {
					if (uc_str[28] == '1' || strncmp(&uc_str[28], "0.95", 4) == 0 || strncmp(&uc_str[28], "0.96", 4) == 0)
						return UCodes::F3DEX;
					else if (uc_str[31] == '2') {
						return UCodes::F3DEX2;
					}
				}
			}

			break;
		}
	}

}