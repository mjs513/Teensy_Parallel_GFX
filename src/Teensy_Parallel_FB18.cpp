
#include "Teensy_Parallel_GFX.h"


//=============================================================================
// 16 bit version of the Frame buffer functions
//=============================================================================

inline uint32_t color565To666(uint16_t color) {
    //        G and B                        R
    return ((color & 0x07FF) << 1) | ((color & 0xF800) << 2);
}

inline uint16_t color666To565(uint32_t color) {
    //        G and B                        R
    return ((color & 0x0FFF) >> 1) | ((color & 0x3E000) >> 2);
}

inline uint32_t color888To666(uint32_t color) {
    //             B (8->6)                G (8->6)
    return ((color & 0xfc) >> 2) | ((color & 0xfc00) >> 4) | ((color & 0xfc0000) >> 6);
}


void Teensy_Parallel_FB18::drawPixel(int16_t x, int16_t y, uint16_t color) {
    updateChangedRange(x, y); // update the range of the screen that has been changed;
    // could probably will assign directly later but as an experiment.
    _pfbtft[y * (int)_width + x] = color565To666(color);
}

void Teensy_Parallel_FB18::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    uint32_t color666 = color565To666(color);
    uint32_t *pfb = &_pfbtft[y * (int)_width + x];
    while (h--) {
        if (y >= _height) break;
        *pfb = color666;
        pfb += _width;
        y++;
    }
}

void Teensy_Parallel_FB18::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    uint32_t color666 = color565To666(color);
    uint32_t *pfb = &_pfbtft[y * (int)_width + x];

    while (w--) {
        if (x >= _width) break;
        *pfb++ = color666;
        x++;
    }
}

// 
void Teensy_Parallel_FB18::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;
    uint32_t color666 = color565To666(color);
    //Serial.printf("FillRect(%d,%d,%d,%d, %x): %u %x %x %x\n", x, y, w, h, color, sizeof(color24), color24.r, color24.g, color24.b);
    uint32_t *pfbRow = &_pfbtft[y * (int)_width + x];
    for (int16_t iy = 0; iy < h; iy++) {
        if ((y+iy) >= _height) break;
        uint32_t *pfb = pfbRow;
        for (int16_t ix = 0; ix < w; ix++) {
            if ((x+ix) >= _width) break;
            *pfb++ = color666;
        }
        pfbRow += _width; // setup for next row
    }
}

void Teensy_Parallel_FB18::writeRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t w_image, const uint16_t *pcolors) {

    uint32_t *pfbRow = &_pfbtft[y * (int)_width + x];
    const uint16_t *pcolors_row = pcolors;
    for (int16_t iy = 0; iy < h; iy++) {
        if ((y+iy) >= _height) break;
        uint32_t *pfb = pfbRow;
        pcolors = pcolors_row;
        for (int16_t ix = 0; ix < w; ix++) {
            if ((x+ix) >= _width) break;
            uint16_t color = *pcolors++;
            *pfb++ = color565To666(color);
        }
        pfbRow += _width; // setup for next row
        pcolors_row += w_image; // setup for next row.
    }
}

void Teensy_Parallel_FB18::writeRect8BPP(int16_t x, int16_t y, int16_t w, int16_t h, 
                                         int16_t w_image, const uint8_t *pcolors, const uint16_t *palette) {

    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;

    // caller already clipped to bounds.
    // also assumes that the pixels is pointing to the first output one. 
    uint32_t *pfbRow = &_pfbtft[y * (int)_width + x];
    const uint8_t *pcolors_row = pcolors;
    //Serial.printf("writeRect8BPP(%d, %d, %d, %d - %d %p %p)\n", x, y, w, h, w_image, pcolors, palette); Serial.flush();
    //Serial.printf("\t%u %u %p %p\n", _width, _height, _pfbtft, pfbRow); Serial.flush();
    
    for (int16_t iy = 0; iy < h; iy++) {
        uint32_t *pfb = pfbRow;
        pcolors = pcolors_row;
        for (int16_t ix = 0; ix < w; ix++) {
            uint16_t color = palette[*pcolors++];
            *pfb++ = color565To666(color);
        }
        pfbRow += _width; // setup for next row
        pcolors_row += w_image; // setup for next row.
    }
}

void Teensy_Parallel_FB18::writeRectNBPP(int16_t x, int16_t y, int16_t w, int16_t h,
                       uint8_t bits_per_pixel, uint16_t count_of_bytes_per_row, uint8_t row_shift_init,
                       const uint8_t *pixels,
                       const uint16_t *palette) {

    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;
    uint32_t *pfbPixel_row = &_pfbtft[y * _width + x];
    uint8_t pixel_bit_mask = (1 << bits_per_pixel) - 1; // get mask to use below
    const uint8_t *pixels_row_start = pixels; // remember our starting position offset into row
    for (; h > 0; h--) {
        uint32_t *pfbPixel = pfbPixel_row;
        pixels = pixels_row_start;            // setup for this row
        uint8_t pixel_shift = row_shift_init; // Setup mask

        for (int i = 0; i < w; i++) {
            uint16_t color = palette[((*pixels) >> pixel_shift) & pixel_bit_mask];
            *pfbPixel++ = color565To666(color);
            if (!pixel_shift) {
                pixel_shift = 8 - bits_per_pixel; // setup next mask
                pixels++;
            } else {
                pixel_shift -= bits_per_pixel;
            }
        }
        pfbPixel_row += _width;
        pixels_row_start += count_of_bytes_per_row;
    }
}

void Teensy_Parallel_FB18::readRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                   uint16_t *pcolors) {
    // Warning this one is not checking that things will fit...
    uint32_t *pfbPixel_row = &_pfbtft[y * _width + x];
    for (; h > 0; h--) {
        uint32_t *pfbPixel = pfbPixel_row;
        for (int i = 0; i < w; i++) {
            *pcolors++ = color666To565(*pfbPixel);
            pfbPixel++;
        }
        pfbPixel_row += _width;
    }
}


//=============================================================================
// 32 bit (RGB8888) version of the Frame buffer functions
//=============================================================================

void Teensy_Parallel_FB18::drawPixel24(int16_t x, int16_t y, uint32_t color) {
    updateChangedRange(x, y); // update the range of the screen that has been changed;
    // could probably will assign directly later but as an experiment.
    _pfbtft[y * (int)_width + x] = color888To666(color);
}

void Teensy_Parallel_FB18::drawFastVLine24(int16_t x, int16_t y, int16_t h, uint32_t color) {
    updateChangedRange(x, y, 1, h); // update the range of the screen that has been changed;

    uint32_t color18 = color888To666(color);
    uint32_t *pfb = &_pfbtft[y * (int)_width + x];
    while (h--) {
        if (y >= _height) break;
        *pfb = color18;
        pfb += _width;
        y++;
    }

}

void Teensy_Parallel_FB18::drawFastHLine24(int16_t x, int16_t y, int16_t w, uint32_t color) {
    updateChangedRange(x, y, w, 1); // update the range of the screen that has been changed;

    uint32_t color18 = color888To666(color);
    uint32_t *pfb = &_pfbtft[y * (int)_width + x];

    while (w--) {
        if (x >= _width) break;
        *pfb = color18;
        x++;
    }

}

void Teensy_Parallel_FB18::fillRect24(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) {
    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;
    uint32_t color18 = color888To666(color);
    Serial.printf("FillRect24(%d,%d,%d,%d, %x): %x\n", x, y, w, h, color, color18);
    uint32_t *pfbRow = &_pfbtft[y * (int)_width + x];
    for (int16_t iy = 0; iy < h; iy++) {
        if ((y+iy) >= _height) break;
        uint32_t *pfb = pfbRow;
        for (int16_t ix = 0; ix < w; ix++) {
            if ((x+ix) >= _width) break;
            *pfb++ = color18;
        }
        pfbRow += _width; // setup for next row
    }
}

void Teensy_Parallel_FB18::writeRect24(int16_t x, int16_t y, int16_t w, int16_t h, int16_t w_image, const uint32_t *pcolors) {
    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;
    uint32_t *pfbRow = &_pfbtft[y * (int)_width + x];
    const uint32_t *pcolors_row = pcolors;
    for (int16_t iy = 0; iy < h; iy++) {
        if ((y+iy) >= _height) break;
        uint32_t *pfb = pfbRow;
        pcolors = pcolors_row;
        for (int16_t ix = 0; ix < w; ix++) {
            if ((x+ix) >= _width) break;
            *pfb++ = color888To666(*pcolors++);
        }
        pfbRow += _width; // setup for next row
        pcolors_row += w_image; // setup for next row.
    }
}

