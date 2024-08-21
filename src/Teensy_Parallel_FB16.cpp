
#include "Teensy_Parallel_GFX.h"


//=============================================================================
// 16 bit version of the Frame buffer functions
//=============================================================================

void Teensy_Parallel_FB16::drawPixel(int16_t x, int16_t y, uint16_t color) {
    updateChangedRange(x, y); // update the range of the screen that has been changed;
    _pfbtft[y * (int)_width + x] = color;
}

void Teensy_Parallel_FB16::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {
    updateChangedRange(x, y, 1, h); // update the range of the screen that has been changed;

    uint16_t *pfb = &_pfbtft[y * (int)_width + x];
    while (h--) {
        *pfb = color;
        pfb += _width;
        y++;
    }
}

void Teensy_Parallel_FB16::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {
    updateChangedRange(x, y, w, 1); // update the range of the screen that has been changed;

    uint16_t *pfb = &_pfbtft[y * (int)_width + x];
    while (w--) {
        *pfb++ = color;
    }
}

// 
void Teensy_Parallel_FB16::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;
    //Serial.printf("fillRect(%d, %d, %d, %d - %d %x)\n", x, y, w, h, color); Serial.flush();
    //Serial.printf("\t%u %u %p\n", _width, _height, _pfbtft); Serial.flush();

    uint16_t *pfbRow = &_pfbtft[y * (int)_width + x];
    for (int16_t iy = 0; iy < h; iy++) {
        if ((y+iy) >= _height) break;
        uint16_t *pfb = pfbRow;
        for (int16_t ix = 0; ix < w; ix++) {
            if ((x+ix) >= _width) break;
            *pfb++ = color;
        }
        pfbRow += _width; // setup for next row
    }
}

void Teensy_Parallel_FB16::writeRect(int16_t x, int16_t y, int16_t w, int16_t h,  int16_t w_image, const uint16_t *pcolors) {
    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;

    uint16_t *pfbRow = &_pfbtft[y * (int)_width + x];
    const uint16_t *pcolors_row = pcolors;
    for (int16_t iy = 0; iy < h; iy++) {
        if ((y+iy) >= _height) break;
        uint16_t *pfb = pfbRow;
        pcolors = pcolors_row;
        for (int16_t ix = 0; ix < w; ix++) {
            if ((x+ix) >= _width) break;
            *pfb++ = *pcolors++;
        }
        pfbRow += _width; // setup for next row
        pcolors_row += w_image; // setup for next row.
    }
}

void Teensy_Parallel_FB16::writeRect8BPP(int16_t x, int16_t y, int16_t w, int16_t h, 
                                         int16_t w_image, const uint8_t *pcolors, const uint16_t *palette) {

    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;

    // caller already clipped to bounds.
    // also assumes that the pixels is pointing to the first output one. 
    uint16_t *pfbRow = &_pfbtft[y * (int)_width + x];
    const uint8_t *pcolors_row = pcolors;
    //Serial.printf("writeRect8BPP(%d, %d, %d, %d - %d %p %p)\n", x, y, w, h, w_image, pcolors, palette); Serial.flush();
    //Serial.printf("\t%u %u %p %p\n", _width, _height, _pfbtft, pfbRow); Serial.flush();
    
    for (int16_t iy = 0; iy < h; iy++) {
        uint16_t *pfb = pfbRow;
        pcolors = pcolors_row;
        for (int16_t ix = 0; ix < w; ix++) {
            *pfb++ = palette[*pcolors++];
        }
        pfbRow += _width; // setup for next row
        pcolors_row += w_image; // setup for next row.
    }
}

void Teensy_Parallel_FB16::writeRectNBPP(int16_t x, int16_t y, int16_t w, int16_t h,
                       uint8_t bits_per_pixel, uint16_t count_of_bytes_per_row, uint8_t row_shift_init,
                       const uint8_t *pixels,
                       const uint16_t *palette) {

    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;
    uint16_t *pfbPixel_row = &_pfbtft[y * _width + x];
    uint8_t pixel_bit_mask = (1 << bits_per_pixel) - 1; // get mask to use below
    const uint8_t *pixels_row_start = pixels; // remember our starting position offset into row
    for (; h > 0; h--) {
        uint16_t *pfbPixel = pfbPixel_row;
        pixels = pixels_row_start;            // setup for this row
        uint8_t pixel_shift = row_shift_init; // Setup mask

        for (int i = 0; i < w; i++) {
            *pfbPixel++ = palette[((*pixels) >> pixel_shift) & pixel_bit_mask];
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


void Teensy_Parallel_FB16::readRect(int16_t x, int16_t y, int16_t w, int16_t h,
                                   uint16_t *pcolors) {
    // Warning this one is not checking that things will fit...
    uint16_t *pfbPixel_row = &_pfbtft[y * _width + x];
    for (; h > 0; h--) {
        uint16_t *pfbPixel = pfbPixel_row;
        for (int i = 0; i < w; i++) {
            *pcolors++ = *pfbPixel++;
        }
        pfbPixel_row += _width;
    }
}


//=============================================================================
// 32 bit (RGB8888) version of the Frame buffer functions
//=============================================================================
inline uint16_t color888To565(uint16_t color) {
    return Teensy_Parallel_GFX::color565(color >>16, color >> 8, color);
}

void Teensy_Parallel_FB16::drawPixel24(int16_t x, int16_t y, uint32_t color) {
    drawPixel(x, y, color888To565(color));
}

void Teensy_Parallel_FB16::drawFastVLine24(int16_t x, int16_t y, int16_t h, uint32_t color) {
    drawFastVLine(x, y, h, color888To565(color));

}

void Teensy_Parallel_FB16::drawFastHLine24(int16_t x, int16_t y, int16_t w, uint32_t color) {
    drawFastHLine(x, y, w, color888To565(color));

}

void Teensy_Parallel_FB16::fillRect24(int16_t x, int16_t y, int16_t w, int16_t h, uint32_t color) {
    fillRect(x, y, w, h, color888To565(color));

}

void Teensy_Parallel_FB16::writeRect24(int16_t x, int16_t y, int16_t w, int16_t h, int16_t w_image, const uint32_t *pcolors) {
    updateChangedRange(x, y, w, h); // update the range of the screen that has been changed;
    uint16_t *pfbRow = &_pfbtft[y * (int)_width + x];
    const uint32_t *pcolors_row = pcolors;
    for (int16_t iy = 0; iy < h; iy++) {
        if ((y+iy) >= _height) break;
        uint16_t *pfb = pfbRow;
        pcolors = pcolors_row;
        for (int16_t ix = 0; ix < w; ix++) {
            if ((x+ix) >= _width) break;
            uint32_t color = *pcolors++;
            *pfb++ = color888To565(color);
        }
        pfbRow += _width; // setup for next row
        pcolors_row += w_image; // setup for next row.
    }
}

