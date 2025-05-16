#pragma once

#include <string.h>
#include <stdbool.h>
#include "memory.h"
#include "cpu.h"
#include "emulator.h"

#define LCD_SIZE 256
#define LCD_DISPLAY_WIDTH 160
#define LCD_DISPLAY_HEIGHT 144
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

typedef struct {
    uint8_t color : 2; // 0 = white, 1 = light grey, 2 = dark grey, 3 = black 
    uint8_t priority : 1;
} PIXEL;

typedef struct {
    uint8_t r, g, b;
} COLOUR;

uint8_t pixelMap[LCD_SIZE][LCD_SIZE] = {0};
SPRITE spriteBuffer[10]; // During mode 2 OAM Scan, sprites to be rendered onto the current scanline are pushed into this buffer
uint8_t spriteBufferIndex = 0;
PIXEL backgroundFIFO[16]; // During mode 3 Draw, pixels in this FIFO are pushed to the LCD
PIXEL spriteFIFO[16]; // During mode 3 Draw, pixels in this FIFO are pushed to the LCD

extern const COLOUR colourPalette[4];
extern COLOUR backgroundPalette[4];
extern COLOUR spritePalette[2][4];
extern COLOUR frameBuffer[LCD_DISPLAY_WIDTH][LCD_DISPLAY_HEIGHT];
extern uint8_t tiles[384][8][8]; // Stored at beginning of VRAM at 0x8000~0x97FF

void ppuStep(void);
PIXEL ppuFetchPixel(void);
void ppuHBlank(void);
void ppuVBlank(void); // Pads out draw time of each scanline to be 456 T-cycles;
void ppuOAMScan(void);
void ppuRenderScanline(void);
void ppuRenderSprite(void);
COLOUR ppuGetColour(uint8_t colourID, uint16_t paletteAddr);
static bool getBit(uint8_t bitPos, uint8_t byte);

