#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif
#include "gbi.h"

#include "gfx_api.h"

#include <stdint.h>

#define C0(pos, width) (((*pcmd)->words.w0 >> (pos)) & ((1U << width) - 1))
#define C1(pos, width) (((*pcmd)->words.w1 >> (pos)) & ((1U << width) - 1))

void InitF3D()
{
    gGfxFunctions[G_MTX] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_matrix(C0(16, 8), (const int32_t*)seg_addr((*pcmd)->words.w1));
    };
    gGfxFunctions[G_POPMTX] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_pop_matrix(1);
    };
    gGfxFunctions[G_MOVEMEM] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_movemem(C0(16, 8), 0, seg_addr((*pcmd)->words.w1));
    };
    gGfxFunctions[(uint8_t) G_MOVEWORD] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_movemem(C0(16, 8), 0, seg_addr((*pcmd)->words.w1));
    };
    gGfxFunctions[(uint8_t) G_TEXTURE] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_texture(C1(16, 16), C1(0, 16), C0(11, 3), C0(8, 3), C0(0, 8));
    };
    gGfxFunctions[G_VTX] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_vertex((C0(0, 16)) / sizeof(Vtx), C0(16, 4), (const Vtx*)seg_addr((*pcmd)->words.w1));
    };
    gGfxFunctions[G_DL] = [](Gfx** pcmd, uintptr_t* pSegAddr)
    {
        if (C0(16, 1) == 0) {
            // Push return address
            gfx_run_dl((Gfx*)seg_addr((*pcmd)->words.w1), (*pcmd)->words.w1);
        }
        else {
            *pSegAddr = (*pcmd)->words.w1;
            *pcmd = (Gfx*)seg_addr(*pSegAddr);
            gfx_dec(pcmd, pSegAddr);
        }
    };
    gGfxFunctions[(uint8_t)G_SETGEOMETRYMODE] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_geometry_mode(0, (*pcmd)->words.w1);
    };
    gGfxFunctions[(uint8_t)G_CLEARGEOMETRYMODE] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_geometry_mode((*pcmd)->words.w1, 0);
    };
    gGfxFunctions[(uint8_t)G_TRI1] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_tri1(C1(16, 8) / 10, C1(8, 8) / 10, C1(0, 8) / 10);
    };
    gGfxFunctions[(uint8_t)G_SETOTHERMODE_L] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_set_other_mode(C0(8, 8), C0(0, 8), (*pcmd)->words.w1);
    };
    gGfxFunctions[(uint8_t)G_SETOTHERMODE_H] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_sp_set_other_mode(C0(8, 8) + 32, C0(0, 8), (uint64_t)(*pcmd)->words.w1 << 32);
    };
    gGfxFunctions[G_SETTIMG] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_texture_image(C0(21, 3), C0(19, 2), C0(0, 10), seg_addr((*pcmd)->words.w1));
    };
    gGfxFunctions[G_LOADBLOCK] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_load_block(C1(24, 3), C0(12, 12), C0(0, 12), C1(12, 12), C1(0, 12));
    };
    gGfxFunctions[G_LOADTILE] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_load_tile(C1(24, 3), C0(12, 12), C0(0, 12), C1(12, 12), C1(0, 12));
    };
    gGfxFunctions[G_SETTILE] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_tile(C0(21, 3), C0(19, 2), C0(9, 9), C0(0, 9), C1(24, 3), C1(20, 4), C1(18, 2), C1(14, 4), C1(10, 4), C1(8, 2), C1(4, 4), C1(0, 4));
    };
    gGfxFunctions[G_SETTILESIZE] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_tile_size(C1(24, 3), C0(12, 12), C0(0, 12), C1(12, 12), C1(0, 12));
    };
    gGfxFunctions[G_LOADTLUT] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_load_tlut(C1(24, 3), C1(14, 10));
    };
    gGfxFunctions[G_SETENVCOLOR] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_env_color(C1(24, 8), C1(16, 8), C1(8, 8), C1(0, 8));
    };
    gGfxFunctions[G_SETPRIMCOLOR] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_prim_color(C1(24, 8), C1(16, 8), C1(8, 8), C1(0, 8));
    };
    gGfxFunctions[G_SETFOGCOLOR] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_fog_color(C1(24, 8), C1(16, 8), C1(8, 8), C1(0, 8));
    };
    gGfxFunctions[G_SETFILLCOLOR] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_fill_color((*pcmd)->words.w1);
    };
    gGfxFunctions[G_SETCOMBINE] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_combine_mode(
            color_comb(C0(20, 4), C1(28, 4), C0(15, 5), C1(15, 3)),
            color_comb(C0(12, 3), C1(12, 3), C0(9, 3), C1(9, 3)));
    };
    gGfxFunctions[G_TEXRECT] = [](Gfx** pcmd, uintptr_t* pSegAddr)
    {
        int32_t lrx = 0, lry = 0, tile = 0, ulx = 0, uly = 0;
        uint32_t uls = 0, ult = 0, dsdx = 0, dtdy = 0;

        lrx = C0(12, 12);
        lry = C0(0, 12);
        tile = C1(24, 3);
        ulx = C1(12, 12);
        uly = C1(0, 12);
        gfx_inc(pcmd, pSegAddr);
        uls = C1(16, 16);
        ult = C1(0, 16);
        gfx_inc(pcmd, pSegAddr);
        dsdx = C1(16, 16);
        dtdy = C1(0, 16);

        gfx_dp_texture_rectangle(ulx, uly, lrx, lry, tile, uls, ult, dsdx, dtdy, 0);
    };
    gGfxFunctions[G_TEXRECTFLIP] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        auto cmd = *(Gfx**)pcmd;
        int32_t lrx = 0, lry = 0, tile = 0, ulx = 0, uly = 0;
        uint32_t uls = 0, ult = 0, dsdx = 0, dtdy = 0;

        lrx = C0(12, 12);
        lry = C0(0, 12);
        tile = C1(24, 3);
        ulx = C1(12, 12);
        uly = C1(0, 12);
        gfx_inc(&cmd, psegAddr);
        uls = C1(16, 16);
        ult = C1(0, 16);
        gfx_inc(&cmd, psegAddr);
        dsdx = C1(16, 16);
        dtdy = C1(0, 16);

        gfx_dp_texture_rectangle(ulx, uly, lrx, lry, tile, uls, ult, dsdx, dtdy, 1);
    };
    gGfxFunctions[G_FILLRECT] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        auto cmd = *(Gfx**)pcmd;
        int32_t lrx = 0, lry = 0, tile = 0, ulx = 0, uly = 0;
        uint32_t uls = 0, ult = 0, dsdx = 0, dtdy = 0;

        lrx = C0(12, 12);
        lry = C0(0, 12);
        tile = C1(24, 3);
        ulx = C1(12, 12);
        uly = C1(0, 12);
        gfx_inc(&cmd, psegAddr);
        uls = C1(16, 16);
        ult = C1(0, 16);
        gfx_inc(&cmd, psegAddr);
        dsdx = C1(16, 16);
        dtdy = C1(0, 16);

        gfx_dp_texture_rectangle(ulx, uly, lrx, lry, tile, uls, ult, dsdx, dtdy, 0);
    };
    gGfxFunctions[G_SETSCISSOR] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_scissor(C1(24, 2), C0(12, 12), C0(0, 12), C1(12, 12), C1(0, 12));
    };
    gGfxFunctions[G_SETZIMG] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_z_image(seg_addr((*pcmd)->words.w1));
    };
    gGfxFunctions[G_SETCIMG] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_set_color_image(C0(21, 3), C0(19, 2), C0(0, 11), seg_addr((*pcmd)->words.w1));
    };
    gGfxFunctions[G_RDPFULLSYNC] = [](Gfx** pcmd, uintptr_t* psegAddr)
    {
        gfx_dp_full_sync();
    };
}
