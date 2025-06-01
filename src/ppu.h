#pragma once

#include <string.h>
#include <stdbool.h>
#include "memory.h"
#include "cpu.h"
// #include "emulator.h"
#include <SDL2/SDL.h>

#define LCD_SIZE 256
#define LCD_DISPLAY_WIDTH 160
#define LCD_DISPLAY_HEIGHT 144
#define PIXEL_SIZE 3
#define SPRITE_MAX 40
#define SPRITE_MAX_SCANLINE 10
#define LCD_FPS 60

// PPU Register Addresses
#define TILE_MAP_0 0x9800 // Location in memory of first 32x32 background/window tilemap
#define TILE_MAP_1 0x9C00 // Location in memory of second 32x32 background/window tilemap
#define LCD_CONTROL 0xFF40 // LCD Control Register
#define LCD_Y 0xFF44 // Current horizontal line
#define LCD_Y_CMP 0xFF45 // Compared against LCD_Y. When equivalent, a STAT interrupt is triggered
#define LCD_STATUS 0xFF41 // LCD status register
#define SCROLL_Y 0xFF42 // Y pixel coordinate of top-left of the viewport
#define SCROLL_X 0xFF43 // X pixel coordinate of top-left of the viewport
#define WINDOW_Y 0xFF4A // Y pixel coordinate of window position wrt viewport
#define WINDOW_X 0xFF4B // X pixel coordinate of window position wrt viewport
#define BACKGROUND_PALETTE 0xFF47 // Assigns gray shades to the color indices of background and window tiles
#define OBJECT_PALETTE_0 0xFF48 // Same as BACKGROUND OBJECT but for object. Lower 2 bits are ignored as color index 0 is transparent
#define OBJECT_PALETTE_1 0xFF49

enum PPUModes {
    Mode_0_HBlank,
    Mode_1_VBlank,
    Mode_2_OAM,
    Mode_3_Draw
};

typedef struct {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
} SDL_OBJ;

typedef struct {
    uint8_t control;
    uint8_t scrollX;
    uint8_t scrollY;
    uint8_t windowX;
    uint8_t windowY;
    uint8_t ly;
    uint32_t tick;
} PPU;

typedef struct {
    uint8_t yPos;
    uint8_t xPos;
    uint8_t index;
    struct {
        uint8_t priority : 1;
        uint8_t yFlip : 1;
        uint8_t xFlip : 1;
        uint8_t palette : 1;
    };
} SPRITE;


const uint16_t colourPalette[4];
extern uint16_t frameBuffer[LCD_DISPLAY_WIDTH * LCD_DISPLAY_HEIGHT];

void ppuInit(void);
void ppuStep(void);// Pads out draw time of each scanline to be 456 T-cycles;
void ppuRenderScanline(void);
void ppuRenderSprite(void);
uint16_t ppuGetColour(uint8_t colourID, uint16_t paletteAddr);
void sdlInitialize(SDL_OBJ *sdl);
void sdlDeinitialize(SDL_OBJ *sdl);
void sdlRender(SDL_OBJ *sdl, uint16_t *pixels);

