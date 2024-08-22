
#include "Teensy_Parallel_GFX.h"


//=============================================================================
// 16 bit version of the Frame buffer functions
//=============================================================================

inline uint32_t color565To888(uint16_t color) {
    uint8_t r, g, b;
    Teensy_Parallel_GFX::color565toRGB(color, r, g, b);
    if (r) r |= 0x7;
    if (g) g |= 0x3;
    if (b) b |= 0x7;
    return 0xff000000 | (r << 16) | (g << 8) | b;
}


void Teensy_Parallel_FB24::drawPixel(int16_t x, int16_t y, uint16_t color) {
    drawPixel24(x, y, color565To888(color));
}

void Teensy_Parallel_FB24::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    drawFastVLine24(x, y, h, color565To888(color));
}

void Teensy_Parallel_FB24::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    drawFastHLine24(x, y, w, color565To888(color));
}

// 
void Teensy_Parallel_FB24::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    fillRect24(x, y, w, h, color565To888(color));
}

void Teensy_Parallel_FB24::writeRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t w_image, const uint16_t *pcolors) {

    uint32_t *pfbRow = &_pfbtft[y * (int)_width + x];
    const uint16_t *pcolors_row = pcolors;
    for (int16_t iy = 0; iy < h; iy++) {
        if ((y+iy) >= _height) break;
        uint32_t *pfb = pfbRow;
        pcolors = pcolors_row;
        for (int16_t ix = 0; ix < w; ix++) {
            if ((x+ix) >= _width) break;
            uint16_t color = *pcolors++;
            *pfb++ = color565To888(color);
        }
        pfbRow += _width; // setup for next row
        pcolors_row += w_image; // setup for next row.
    }
}

void Teensy_Parallel_FB24::writeRect8BPP(int16_t x, int16_t y, int16_t w, int16_t h, 
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
            *pfb++ = color565To888(palette[*pcolors++]);
        }
        pfbRow += _width; // setup for next row
        pcolors_row += w_image; // setup for next row.
    }
}

void Teensy_Parallel_FB24::writeRectNBPP(int16_t x, int16_t y, int16_t w, int16_t h,
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
            *pfbPixel++ = color565To888(palette[((*pixels) >> pixel_shift) & pixel_bit_mask]);
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

void Teensy_Parallel_FB24::readRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                   uint16_t *pcolors) {
    // Warning this one is not checking that things will fit...
    uint32_t *pfbPixel_row = &_pfbtft[y * _width + x];
    for (; h > 0; h--) {
        uint32_t *pfbPixel = pfbPixel_row;
        for (int i = 0; i < w; i++) {
            *pcolors++ = Teensy_Parallel_GFX::color888To565(*pfbPixel++);
        }
        pfbPixel_row += _width;
    }
}


//=============================================================================
// 32 bit (RGB8888) version of the Frame buffer functions
//=============================================================================

void Teensy_Parallel_FB24::drawPixel24(int16_t x, int16_t y, uint32_t color) {
    updateChangedRange(x, y); // update the range of the screen that has been changed;
    _pfbtft[y * (int)_width + x] = color;
}

void Teensy_Parallel_FB24::drawFastVLine24(int16_t x, int16_t y, int16_t h, uint32_t color) {
    updateChangedRange(x, y, 1, h); // update the range of the screen that has been changed;

    uint32_t *pfb = &_pfbtft[y * (int)_width + x];
    while (h--) {
        if (y >= _height) break;
        *pfb = color;
        pfb += _width;
        y++;
    }

}

void Teensy_Parallel_FB24::drawFastHLine24(int16_t x, int16_t y, int16_t w, uint32_t color) {
    updateChangedRange(x, y, w, 1); // update the range of the screen that has been changed;

    uint32_t *pfb = &_pfbtft[y * (int)_width + x];

    while (w--) {
        if (x >= _width) break;
        *pfb++ = color;
        x++;
    }

}

void Teensy_Parallel_FB24::fillRect24(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) {
    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;
    uint32_t *pfbRow = &_pfbtft[y * (int)_width + x];
    for (int16_t iy = 0; iy < h; iy++) {
        if ((y+iy) >= _height) break;
        uint32_t *pfb = pfbRow;
        for (int16_t ix = 0; ix < w; ix++) {
            if ((x+ix) >= _width) break;
            *pfb++ = color;
        }
        pfbRow += _width; // setup for next row
    }
}

void Teensy_Parallel_FB24::writeRect24(int16_t x, int16_t y, int16_t w, int16_t h, int16_t w_image, const uint32_t *pcolors) {
    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;
    uint32_t *pfbRow = &_pfbtft[y * (int)_width + x];
    const uint32_t *pcolors_row = pcolors;
    for (int16_t iy = 0; iy < h; iy++) {
        if ((y+iy) >= _height) break;
        uint32_t *pfb = pfbRow;
        pcolors = pcolors_row;
        for (int16_t ix = 0; ix < w; ix++) {
            if ((x+ix) >= _width) break;
            *pfb++ = *pcolors++;
        }
        pfbRow += _width; // setup for next row
        pcolors_row += w_image; // setup for next row.
    }
}

