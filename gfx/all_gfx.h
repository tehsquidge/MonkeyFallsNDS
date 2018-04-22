// Graphics converted using PAGfx by Mollusk.

#pragma once

#include <PA_BgStruct.h>

#ifdef __cplusplus
extern "C"{
#endif

// Sprites:
extern const unsigned char tukituki_Sprite[1024] _GFX_ALIGN; // Palette: sprites_Pal
extern const unsigned char monkey_Sprite[768] _GFX_ALIGN; // Palette: sprites_Pal
extern const unsigned char play_Sprite[4096] _GFX_ALIGN; // Palette: sprites_Pal
extern const unsigned char gameover_Sprite[4096] _GFX_ALIGN; // Palette: sprites_Pal
extern const unsigned char paused_Sprite[8192] _GFX_ALIGN; // Palette: sprites_Pal

// Backgrounds:
extern const PA_BgStruct bottom;
extern const PA_BgStruct top;
extern const PA_BgStruct title;

// Palettes:
extern const unsigned short sprites_Pal[256] _GFX_ALIGN;

#ifdef __cplusplus
}
#endif
