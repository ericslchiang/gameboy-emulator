#include "ppu.h"

const uint16_t colourPalette[4] = {
    0xFFFF, // White
    0xAAAA, // Light grey
    0x5555, // Dark grey
    0x000, // Black
};
uint16_t frameBuffer[LCD_DISPLAY_WIDTH * LCD_DISPLAY_HEIGHT];
PPU ppu;

void ppuStep(void) {
    static ppuMode = Mode_0_HBlank;
    static int32_t prevTick = 0;

    ppu.tick += ticks - prevTick;
    prevTick = ticks;

    switch (ppuMode) {
        case Mode_0_HBlank:
            if (ppu.tick >= 204) {
                ppu.ly++;
                if (ppu.ly == 143) {
                    // Implement interrupts
                    ppuMode = Mode_1_VBlank;
                } else {
                    ppuMode = Mode_2_OAM;
                    ppu.tick -= 204;
                }
            } 
            break;
        case Mode_1_VBlank:
            if (ppu.tick >= 456) {
                ppu.ly++;

                if (ppu.ly > 153) {
                    ppuMode = Mode_2_OAM;
                    ppu.ly = 0;
                }
                ppu.tick -= 456;
            }
            break;
        case Mode_2_OAM:
            if (ppu.tick >= 80) {
                ppuMode = Mode_3_Draw;
                ppu.tick -= 80;
            }
            break;
        case Mode_3_Draw:
            if (ppu.tick >= 172) {
                ppuMode = Mode_0_HBlank;
                ppuRenderScanline();
                ppu.tick -= 172;
            }
            break;
    }
}

void ppuRenderScanline(void) {
    bool inWindows = getBit(5, ppu.control) && ppu.windowY <= ppu.ly; // Check if we are rendering a window tile
    uint16_t tileMap;
    if (inWindows) {
        tileMap = getBit(6, ppu.control) ? TILE_MAP_1 : TILE_MAP_0;
    } else {
        tileMap = getBit(3, ppu.control) ? TILE_MAP_1 : TILE_MAP_0;
    }

    uint16_t fetcherY = inWindows ? ppu.ly - ppu.windowY : (ppu.ly + ppu.scrollY) & 255; // Y pixel position in tilemap
    uint16_t tileYOffset = (fetcherY >> 3) << 5; // y line position * 32 tiles/line  

    // Render the scanline
    for (int8_t i = 0; i < 160; i++) {
        uint16_t fetcherX = ppu.scrollX + i; // X pixel position in the viewport
        if (inWindows && i >= ppu.windowX) { // change fetcherX to windows tilemap if inside windows
            fetcherX = i - ppu.windowX;
        }

        // Fetcher X and Y are pixel positions that allows us to calculate the required tile
        // They are both modulo'd with 256 which wraps them around to the beginning of the tilemap 
        // To get the tile position, they can be fetcher/8 to find their tile position in the 32x32 tilemap

        // Get tile address. Base tilemap addr + y tile offest + x tile offset    
        uint8_t tileIndex = memoryRead(tileMap + tileYOffset + ((fetcherX & 255) >> 3));

        //Check if we are using signed or unsigned addressing for fetching tile data from VRAM
        uint16_t tileAddr;
        if (getBit(4, ppu.control)) { //unsigned
            tileAddr = VRAM + tileIndex << 4; // 16x required as each tile is 16 bytes
        } else { //signed
            // +128 required as signed addressing means we need to start from middle of memory
            // 128 * 16 = 2048 = (1/2) of 4096 byte memory block
            tileAddr = 0x8800 + ((int8_t)tileIndex + 128) << 4; 
        }

        // Calculate the correct byte to get from the tile
        // fetcher & 7: y position of correct line of pixels
        // << 1: two bytes per line of pixels
        uint8_t line = (fetcherY & 7) << 1;

        // Fetch the two bytes of color data
        uint8_t tileDataLower = memoryRead(tileAddr + line); 
        uint8_t tileDataUpper = memoryRead(tileAddr + line + 1);

        uint8_t colourBit = 7 - (fetcherX & 7); // Bit 7 of tileData represent leftmost pixel
        uint8_t colourID = getBit(colourBit, tileDataUpper) << 1 | getBit(colourBit, tileDataLower);
        
        // write pixel into frame buffer
        frameBuffer[ppu.ly * LCD_DISPLAY_WIDTH + i] = ppuGetColour(colourID, 0xFF47); // 0xFF47 is palette used for background/window
    }
}

// This function takes a colourID from the current tile and fetches a colour from a changing palette using it
uint16_t ppuGetColour(uint8_t colourID, uint16_t paletteAddr) {
    uint8_t palette = memoryRead(paletteAddr);
    uint8_t colourIndex = getBit(colourID * 2 + 1, palette) << 1 | getBit(colourID * 2, palette);

    return colourPalette[colourIndex];
}


void ppuRenderSprite(void) {
    // Check if sprites are 8x8 or 8x16 pixels
    uint8_t spriteYSize= getBit(2, ppu.control) ? 16 : 8;

    for (int i = 0; i < SPRITE_MAX; i++) {
        SPRITE sprite;

        uint8_t memoryOffset = i << 2; // Each sprite is 4 bytes in OAM
        sprite.yPos = memoryRead(OAM + memoryOffset) - 16; // -16 brings sprite into top edge of screen
        sprite.xPos = memoryRead(OAM + memoryOffset + 1) - 8; // -8 bring sprite into left edge of screen
        sprite.index = memoryRead(OAM + memoryOffset + 2); // Used to calculate address of sprite data in VRAM

        uint8_t spriteAttributes = memoryRead(OAM + memoryOffset + 3);
        sprite.priority = getBit(7, spriteAttributes);
        sprite.yFlip = getBit(6, spriteAttributes);
        sprite.xFlip = getBit(5, spriteAttributes);
        sprite.palette = getBit(4, spriteAttributes);

        // If current scanline lands on the sprite
        if ((ppu.ly >= sprite.yPos) && (ppu.ly < sprite.yPos + spriteYSize)) {
            // the horizontal line (byte) in the sprite to read
            uint8_t line = ppu.ly - sprite.yPos;

            // If sprite is flipped vertically read in sprite data backwards
            if (sprite.yFlip) {
                line = spriteYSize - line;
            }

            // Each horizontal line in the sprite takes 2 bytes to render
            line *= 2; 

            uint16_t spriteAddr = VRAM + line + sprite.index * 16; // *16 each sprite is 16 bytes
            uint8_t spriteDataLower = memoryRead(spriteAddr);
            uint8_t spriteDataUpper = memoryRead(spriteAddr + 1);

            // Bit 7 represents leftmost pixel of sprite, and bit 0 the rightmost 
            for (int j = 7; j >= 0; j++) {
                // If sprite is within the viewport and not out-prioritized by the background/window
                if (
                    sprite.xPos + 7 - j > 160 || 
                    sprite.xPos + 7 - j <= 0 ||
                    // Background take higher priority and background colour is not white
                    (sprite.priority && frameBuffer[sprite.xPos + 7 - j + ppu.ly * LCD_DISPLAY_WIDTH] != 0xFFFF)
                ) {
                    continue;
                }
                uint8_t colourBit = j;
                if (sprite.xFlip) {
                    colourBit = 7 - colourBit;
                }

                uint8_t colourID = getBit(colourBit, spriteDataUpper) << 1 | getBit(colourBit, spriteDataLower);
                uint16_t colourAddr = sprite.palette ? 0xFF49 : 0xFF48;
                uint16_t colour = ppuGetColour(colourID, colourAddr);
                
                // Sprite pixels of WHITE are treated as transparent. White has values {255, 255, 255}.
                if (colour == 0xFFFF) {
                    continue;
                }
                frameBuffer[sprite.xPos + 7 - j + ppu.ly * LCD_DISPLAY_WIDTH] = colour;
            }
        }
    }
}

/*
Some notes about PPU rendering
-PPU chooses whether to get tile from background or window tilemap
-Each tilemap is 32x32 (=1024) tiles
-Tilemap occupies region in memory of ~0x400 (aka 1024 bytes)
-Each byte signifies the address of the tile in memory to be found 
    in VRAM (0x8000 ~ 0x97FF)

-PPU keeps track of which pixel its rendering on the screeen
-Current pixel y-position is given by LY register + SCY
-Current pixel x-position is recorded for each pixel render
-By calculating PPU x-pos / 8
*/

static bool getBit(uint8_t bitPos, uint8_t byte) {
    return byte & (1 << bitPos) ? 1 : 0;
}

// Initializing SDL objects
void sdlInitialize(SDL_OBJ *sdl) {
    sdl->window = SDL_CreateWindow("Gameboy Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, LCD_DISPLAY_WIDTH * PIXEL_SIZE, LCD_DISPLAY_HEIGHT * PIXEL_SIZE, SDL_WINDOW_SHOWN);
    sdl->renderer = SDL_CreateRenderer(sdl->window, -1, SDL_RENDERER_ACCELERATED);
    sdl->texture = SDL_CreateTexture(sdl->renderer, SDL_PIXELFORMAT_ARGB4444, SDL_TEXTUREACCESS_STREAMING, LCD_DISPLAY_WIDTH, LCD_DISPLAY_HEIGHT);
    SDL_SetRenderDrawColor(sdl->renderer, 0, 0, 0, 0);
    SDL_RenderClear(sdl->renderer);
}

// De-initializing SDL objects
void sdlDeinitialize(SDL_OBJ *sdl) {
    SDL_DestroyTexture(sdl->texture);
    SDL_DestroyRenderer(sdl->renderer);
    SDL_DestroyWindow(sdl->window);
    SDL_Quit();
}

void sdlRender(SDL_OBJ *sdl, uint16_t *pixels) {
    SDL_UpdateTexture(sdl->texture, NULL, pixels, 2 * LCD_DISPLAY_WIDTH);
    SDL_RenderCopy(sdl->renderer, sdl->texture, NULL, NULL);
    SDL_RenderPresent(sdl->renderer);
}