// https://forum.pjrc.com/index.php?threads/ili948x_t41_p-a-parallel-display-driver-for-teensy-4-1.72660/

/***************************************************
  This is our library for the Adafruit  ILI9488 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

// <SoftEgg>

// Additional graphics routines by Tim Trzepacz, SoftEgg LLC added December 2015
//(And then accidentally deleted and rewritten March 2016. Oops!)
// Gradient support
//----------------
//		fillRectVGradient	- fills area with vertical gradient
//		fillRectHGradient	- fills area with horizontal gradient
//		fillScreenVGradient - fills screen with vertical gradient
//  	fillScreenHGradient - fills screen with horizontal gradient

// Additional Color Support
//------------------------
//		color565toRGB		- converts 565 format 16 bit color to RGB
//		color565toRGB14		- converts 16 bit 565 format color to 14 bit RGB (2 bits clear for math and sign)
//		RGB14tocolor565		- converts 14 bit RGB back to 16 bit 565 format color

// Low Memory Bitmap Support
//-------------------------
//  writeRect8BPP - 	write 8 bit per pixel paletted bitmap
//  writeRect4BPP - 	write 4 bit per pixel paletted bitmap
//  writeRect2BPP - 	write 2 bit per pixel paletted bitmap
//  writeRect1BPP - 	write 1 bit per pixel paletted bitmap

// String Pixel Length support
//---------------------------
//		strPixelLen			- gets pixel length of given ASCII string

// <\SoftEgg>

#ifndef _TEENSY_ILI9488P_GFX_H
#define _TEENSY_ILI9488P_GFX_H

#define ENABLE_FRAMEBUFFER

#ifdef __cplusplus
#include "Arduino.h"
#include "ILI9341_fonts.h"
#include <stdint.h>

#define CL(_r, _g, _b) ((((_r) & 0xF8) << 8) | (((_g) & 0xFC) << 3) | ((_b) >> 3))

#define sint16_t int16_t

// Map fonts that were modified back to the ILI9341 font
#define ILI948x_t4_mm_font_t ILI9341_t3_font_t

// Lets see about supporting Adafruit fonts as well?
#if __has_include(<gfxfont.h>)
#include <gfxfont.h>
#endif

// Lets see about supporting Adafruit fonts as well?
#ifndef _GFXFONT_H_
#define _GFXFONT_H_

/// Font data stored PER GLYPH
typedef struct {
    uint16_t bitmapOffset; ///< Pointer into GFXfont->bitmap
    uint8_t width;         ///< Bitmap dimensions in pixels
    uint8_t height;        ///< Bitmap dimensions in pixels
    uint8_t xAdvance;      ///< Distance to advance cursor (x axis)
    int8_t xOffset;        ///< X dist from cursor pos to UL corner
    int8_t yOffset;        ///< Y dist from cursor pos to UL corner
} GFXglyph;

/// Data stored for FONT AS A WHOLE
typedef struct {
    uint8_t *bitmap;  ///< Glyph bitmaps, concatenated
    GFXglyph *glyph;  ///< Glyph array
    uint8_t first;    ///< ASCII extents (first char)
    uint8_t last;     ///< ASCII extents (last char)
    uint8_t yAdvance; ///< Newline distance (y axis)
} GFXfont;

#endif // _GFXFONT_H_

// These enumerate the text plotting alignment (reference datum point)
#define TL_DATUM 0 // Top left (default)
#define TC_DATUM 1 // Top centre
#define TR_DATUM 2 // Top right
#define ML_DATUM 3 // Middle left
#define CL_DATUM 3 // Centre left, same as above
#define MC_DATUM 4 // Middle centre
#define CC_DATUM 4 // Centre centre, same as above
#define MR_DATUM 5 // Middle right
#define CR_DATUM 5 // Centre right, same as above
#define BL_DATUM 6 // Bottom left
#define BC_DATUM 7 // Bottom centre
#define BR_DATUM 8 // Bottom right
// #define L_BASELINE  9 // Left character baseline (Line the 'A' character would sit on)
// #define C_BASELINE 10 // Centre character baseline
// #define R_BASELINE 11 // Right character baseline

// Color definitions
#define ILI9488_BLACK 0x0000       /*   0,   0,   0 */
#define ILI9488_NAVY 0x000F        /*   0,   0, 128 */
#define ILI9488_DARKGREEN 0x03E0   /*   0, 128,   0 */
#define ILI9488_DARKCYAN 0x03EF    /*   0, 128, 128 */
#define ILI9488_MAROON 0x7800      /* 128,   0,   0 */
#define ILI9488_PURPLE 0x780F      /* 128,   0, 128 */
#define ILI9488_OLIVE 0x7BE0       /* 128, 128,   0 */
#define ILI9488_LIGHTGREY 0xC618   /* 192, 192, 192 */
#define ILI9488_DARKGREY 0x7BEF    /* 128, 128, 128 */
#define ILI9488_BLUE 0x001F        /*   0,   0, 255 */
#define ILI9488_GREEN 0x07E0       /*   0, 255,   0 */
#define ILI9488_CYAN 0x07FF        /*   0, 255, 255 */
#define ILI9488_RED 0xF800         /* 255,   0,   0 */
#define ILI9488_MAGENTA 0xF81F     /* 255,   0, 255 */
#define ILI9488_YELLOW 0xFFE0      /* 255, 255,   0 */
#define ILI9488_WHITE 0xFFFF       /* 255, 255, 255 */
#define ILI9488_ORANGE 0xFD20      /* 255, 165,   0 */
#define ILI9488_GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define ILI9488_PINK 0xF81F

class Teensy_Parallel_GFX : public Print {

  public:
    Teensy_Parallel_GFX(int16_t w, int16_t h);
    void pushPixels16bit(const uint16_t *pcolors, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {};
    void pushPixels16bitDMA(const uint16_t *pcolors, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {};

    virtual void setAddr(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {};
    virtual void beginWrite16BitColors() {};
    virtual void write16BitColor(uint16_t color) {};
    virtual void endWrite16BitColors() {};
    virtual void write16BitColor(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, const uint16_t *pcolors, uint16_t count) {};
    virtual void fillRectFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {};
    virtual void readRectFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors) {};
    virtual bool writeRectAsyncFlexIO(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors) {return false;}
    virtual bool writeRectAsyncActiveFlexIO() { return false; }
    virtual void setRotation(uint8_t r) {};

    // setClipRect() sets a clipping rectangle (relative to any set origin) for drawing to be limited to.
    // Drawing is also restricted to the bounds of the display

    void setClipRect(int16_t x1, int16_t y1, int16_t w, int16_t h) {
        _clipx1 = x1;
        _clipy1 = y1;
        _clipx2 = x1 + w;
        _clipy2 = y1 + h;
        // if (Serial) Serial.printf("Set clip Rect %d %d %d %d\n", x1, y1, w, h);
        updateDisplayClip();
    }
    void setClipRect() {
        _clipx1 = 0;
        _clipy1 = 0;
        _clipx2 = _width;
        _clipy2 = _height;
        // if (Serial) Serial.printf("clear clip Rect\n");
        updateDisplayClip();
    }

    // setOrigin sets an offset in display pixels where drawing to (0,0) will appear
    // for example: setOrigin(10,10); drawPixel(5,5); will cause a pixel to be drawn at hardware pixel (15,15)
    void setOrigin(int16_t x = 0, int16_t y = 0) {
        _originx = x;
        _originy = y;
        // if (Serial) Serial.printf("Set Origin %d %d\n", x, y);
        updateDisplayClip();
    }
    void getOrigin(int16_t *x, int16_t *y) {
        *x = _originx;
        *y = _originy;
    }

    int16_t height(void) { return _height; }
    int16_t width(void) { return _width; }
    // Pass 8-bit (each) R,G,B, get back 16-bit packed color
    static uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
        return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
    }

    // color565toRGB		- converts 565 format 16 bit color to RGB
    static void color565toRGB(uint16_t color, uint8_t &r, uint8_t &g,
                              uint8_t &b) {
        r = (color >> 8) & 0x00F8;
        g = (color >> 3) & 0x00FC;
        b = (color << 3) & 0x00F8;
    }

    // color565toRGB14		- converts 16 bit 565 format color to 14 bit RGB (2
    // bits clear for math and sign)
    // returns 00rrrrr000000000,00gggggg00000000,00bbbbb000000000
    // thus not overloading sign, and allowing up to double for additions for
    // fixed point delta
    static void color565toRGB14(uint16_t color, int16_t &r, int16_t &g,
                                int16_t &b) {
        r = (color >> 2) & 0x3E00;
        g = (color << 3) & 0x3F00;
        b = (color << 9) & 0x3E00;
    }

    // RGB14tocolor565		- converts 14 bit RGB back to 16 bit 565 format
    // color
    static uint16_t RGB14tocolor565(int16_t r, int16_t g, int16_t b) {
        return (((r & 0x3E00) << 2) | ((g & 0x3F00) >> 3) | ((b & 0x3E00) >> 9));
    }

    // from Adafruit_GFX.h
    void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillScreen(uint16_t color);
    void drawRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
    void fillRoundRect(int16_t x0, int16_t y0, int16_t w, int16_t h, int16_t radius, uint16_t color);
    void drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void drawCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, uint16_t color);
    void fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color);
    void fillCircleHelper(int16_t x0, int16_t y0, int16_t r, uint8_t cornername, int16_t delta, uint16_t color);
    void drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color);
    void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    // Added functions to read pixel data...
    uint16_t readPixel(int16_t x, int16_t y);
    void readRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t *pcolors);
    void writeRect(int16_t x, int16_t y, int16_t w, int16_t h, const uint16_t *pcolors);
    void writeSubImageRect(int16_t x, int16_t y, int16_t w, int16_t h,
                           int16_t image_offset_x, int16_t image_offset_y, int16_t image_width,
                           int16_t image_height, const uint16_t *pcolors);

    virtual size_t write(uint8_t);
    virtual size_t write(const uint8_t *buffer, size_t size);
    uint8_t getRotation(void);
    int16_t getCursorX(void) const { return cursor_x; }
    int16_t getCursorY(void) const { return cursor_y; }
    void setFont(const ILI9341_t3_font_t &f);
    void setFont(const GFXfont *f = NULL);
    void setFontAdafruit(void) { setFont(); }
    void drawFontChar(unsigned int c);
    void drawGFXFontChar(unsigned int c);

    void getTextBounds(const uint8_t *buffer, uint16_t len, int16_t x, int16_t y,
                       int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
    void getTextBounds(const char *string, int16_t x, int16_t y,
                       int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
    void getTextBounds(const String &str, int16_t x, int16_t y,
                       int16_t *x1, int16_t *y1, uint16_t *w, uint16_t *h);
    int16_t strPixelLen(const char *str, uint16_t cb=0xffff);

    uint32_t fetchpixel(const uint8_t *p, uint32_t index, uint32_t x);
    void drawFontPixel(uint8_t alpha, uint32_t x, uint32_t y);

    // added support for drawing strings/numbers/floats with centering
    // modified from tft_ili9488_ESP github library
    // Handle numbers
    int16_t drawNumber(long long_num, int poX, int poY);
    int16_t drawFloat(float floatNumber, int decimal, int poX, int poY);
    // Handle char arrays
    int16_t drawString(const String &string, int poX, int poY);
    int16_t drawString(const char string[], int16_t len, int poX, int poY);

    void setTextDatum(uint8_t datum);

    // added support for scrolling text area
    // https://github.com/vitormhenrique/ILI9488_t3
    // Discussion regarding this optimized version:
    // http://forum.pjrc.com/threads/26305-Highly-optimized-ILI9488-%28320x240-TFT-color-display%29-library
    //
    void setScrollTextArea(int16_t x, int16_t y, int16_t w, int16_t h);
    void setScrollBackgroundColor(uint16_t color);
    void enableScroll(void);
    void disableScroll(void);
    void scrollTextArea(uint8_t scrollSize);
    void resetScrollBackgroundColor(uint16_t color);

    void drawBitmap(int16_t x, int16_t y, const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color);
    void drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size_x, uint8_t size_y);
    void inline drawChar(int16_t x, int16_t y, unsigned char c, uint16_t color, uint16_t bg, uint8_t size) { drawChar(x, y, c, color, bg, size); }
    static const int16_t CENTER = 9998;
    void setCursor(int16_t x, int16_t y, bool autoCenter = false);
    void getCursor(int16_t *x, int16_t *y);
    void setTextColor(uint16_t c);
    void setTextColor(uint16_t c, uint16_t bg);
    void setTextSize(uint8_t sx, uint8_t sy);
    void inline setTextSize(uint8_t s) { setTextSize(s, s); }
    uint8_t getTextSizeX();
    uint8_t getTextSizeY();
    uint8_t getTextSize();
    void setTextWrap(boolean w);
    boolean getTextWrap();
    void sleep(bool enable);

    // writeRect8BPP - 	write 8 bit per pixel paletted bitmap
    //					bitmap data in array at pixels, one byte per
    // pixel
    //					color palette data in array at palette
    void writeRect8BPP(int16_t x, int16_t y, int16_t w, int16_t h,
                       const uint8_t *pixels, const uint16_t *palette);

    // writeRect4BPP - 	write 4 bit per pixel paletted bitmap
    //					bitmap data in array at pixels, 4 bits per
    // pixel
    //					color palette data in array at palette
    //					width must be at least 2 pixels
    void writeRect4BPP(int16_t x, int16_t y, int16_t w, int16_t h,
                       const uint8_t *pixels, const uint16_t *palette);

    // writeRect2BPP - 	write 2 bit per pixel paletted bitmap
    //					bitmap data in array at pixels, 4 bits per
    // pixel
    //					color palette data in array at palette
    //					width must be at least 4 pixels
    void writeRect2BPP(int16_t x, int16_t y, int16_t w, int16_t h,
                       const uint8_t *pixels, const uint16_t *palette);

    // writeRect1BPP - 	write 1 bit per pixel paletted bitmap
    //					bitmap data in array at pixels, 4 bits per
    // pixel
    //					color palette data in array at palette
    //					width must be at least 8 pixels
    void writeRect1BPP(int16_t x, int16_t y, int16_t w, int16_t h,
                       const uint8_t *pixels, const uint16_t *palette);

    // writeRectNBPP - 	write N(1, 2, 4, 8) bit per pixel paletted bitmap
    //					bitmap data in array at pixels
    //  Currently writeRect1BPP, writeRect2BPP, writeRect4BPP use this to do all
    //  of the work.
    //
    void writeRectNBPP(int16_t x, int16_t y, int16_t w, int16_t h,
                       uint8_t bits_per_pixel, const uint8_t *pixels,
                       const uint16_t *palette);

    void fillRectHGradient(int16_t x, int16_t y, int16_t w, int16_t h,
                           uint16_t color1, uint16_t color2);
    void fillRectVGradient(int16_t x, int16_t y, int16_t w, int16_t h,
                           uint16_t color1, uint16_t color2);
    void fillScreenVGradient(uint16_t color1, uint16_t color2);
    void fillScreenHGradient(uint16_t color1, uint16_t color2);
    void setFrameBuffer(uint16_t *frame_buffer);
    uint8_t useFrameBuffer(boolean b);  // use the frame buffer?  First call will allocate
    void freeFrameBuffer(void); // explicit call to release the buffer
    void updateScreen(void);    // call to say update the screen now.
    bool updateScreenAsync(bool update_cont = false); // call to say update the
                                                    // screen optinoally turn
                                                    // into continuous mode.
    void waitUpdateAsyncComplete(void);
    void endUpdateAsync(); // Turn of the continueous mode fla
    boolean asyncUpdateActive(void);

#ifdef ENABLE_FRAMEBUFFER
    uint16_t *getFrameBuffer() { return _pfbtft; }

  

#endif

protected:
    int16_t WIDTH;
    int16_t HEIGHT;
    int16_t _width, _height;

    int16_t cursor_x, cursor_y;
    bool _center_x_text = false;
    bool _center_y_text = false;

    int16_t _clipx1, _clipy1, _clipx2, _clipy2;
    int16_t _originx, _originy;
    int16_t _displayclipx1, _displayclipy1, _displayclipx2, _displayclipy2;
    bool _invisible = false;
    bool _standard = true; // no bounding rectangle or origin set.

    inline void updateDisplayClip() {
        _displayclipx1 = max(0, min(_clipx1 + _originx, width()));
        _displayclipx2 = max(0, min(_clipx2 + _originx, width()));

        _displayclipy1 = max(0, min(_clipy1 + _originy, height()));
        _displayclipy2 = max(0, min(_clipy2 + _originy, height()));
        _invisible = (_displayclipx1 == _displayclipx2 || _displayclipy1 == _displayclipy2);
        _standard = (_displayclipx1 == 0) && (_displayclipx2 == _width) && (_displayclipy1 == 0) && (_displayclipy2 == _height);
        if (Serial) {
            //    Serial.printf("UDC(inline) (%d %d)-(%d %d) %d %d\n", _displayclipx1, _displayclipy1, _displayclipx2,
            //                  _displayclipy2, _invisible, _standard);
        }
    }

    uint16_t textcolor, textbgcolor, scrollbgcolor;
    uint8_t textsize, textsize_x, textsize_y, rotation, textdatum;
    uint32_t textcolorPrexpanded, textbgcolorPrexpanded;
    boolean wrap; // If set, 'wrap' text at right edge of display
    const ILI9341_t3_font_t *font;

    // Anti-aliased font support
    uint8_t fontbpp = 1;
    uint8_t fontbppindex = 0;
    uint8_t fontbppmask = 1;
    uint8_t fontppb = 8;
    uint8_t *fontalphalut;
    float fontalphamx = 1;

    uint32_t padX;
    int16_t scroll_x, scroll_y, scroll_width, scroll_height;
    boolean scrollEnable, isWritingScrollArea; // If set, 'wrap' text at right edge of display

#ifdef ENABLE_FRAMEBUFFER
  // Add support for optional frame buffer
  uint16_t *_pfbtft;              // Optional Frame buffer
  uint8_t _use_fbtft;             // Are we in frame buffer mode?
  uint16_t *_we_allocated_buffer; // We allocated the buffer;
  int16_t _changed_min_x, _changed_max_x, _changed_min_y, _changed_max_y;
  bool _updateChangedAreasOnly = false; // current default off,

  void clearChangedRange() {
    _changed_min_x = 0x7fff;
    _changed_max_x = -1;
    _changed_min_y = 0x7fff;
    _changed_max_y = -1;
  }

  void updateChangedRange(int16_t x, int16_t y, int16_t w, int16_t h)
      __attribute__((always_inline)) {
    if (x < _changed_min_x)
      _changed_min_x = x;
    if (y < _changed_min_y)
      _changed_min_y = y;
    x += w - 1;
    y += h - 1;
    if (x > _changed_max_x)
      _changed_max_x = x;
    if (y > _changed_max_y)
      _changed_max_y = y;
    //if (Serial)Serial.printf("UCR(%d %d %d %d) min:%d %d max:%d %d\n", w, y, w, h, _changed_min_x, _changed_min_y, _changed_max_x, _changed_max_y);
  }

  // could combine with above, but avoids the +-...
  void updateChangedRange(int16_t x, int16_t y) __attribute__((always_inline)) {
    if (x < _changed_min_x)
      _changed_min_x = x;
    if (y < _changed_min_y)
      _changed_min_y = y;
    if (x > _changed_max_x)
      _changed_max_x = x;
    if (y > _changed_max_y)
      _changed_max_y = y;
  }
#endif
             // GFX Font support
    const GFXfont *gfxFont = nullptr;
    int8_t _gfxFont_min_yOffset = 0;

    // Opaque font chracter overlap?
    unsigned int _gfx_c_last;
    int16_t _gfx_last_cursor_x, _gfx_last_cursor_y;
    int16_t _gfx_last_char_x_write = 0;
    uint16_t _gfx_last_char_textcolor;
    uint16_t _gfx_last_char_textbgcolor;
    bool gfxFontLastCharPosFG(int16_t x, int16_t y);

    /**
     * Found in a pull request for the Adafruit framebuffer library. Clever!
     * https://github.com/tricorderproject/arducordermini/pull/1/files#diff-d22a481ade4dbb4e41acc4d7c77f683d
     * Converts  0000000000000000rrrrrggggggbbbbb
     *     into  00000gggggg00000rrrrr000000bbbbb
     * with mask 00000111111000001111100000011111
     * This is useful because it makes space for a parallel fixed-point multiply
     * This implements the linear interpolation formula: result = bg * (1.0 - alpha) + fg * alpha
     * This can be factorized into: result = bg + (fg - bg) * alpha
     * alpha is in Q1.5 format, so 0.0 is represented by 0, and 1.0 is represented by 32
     * @param	fg		Color to draw in RGB565 (16bit)
     * @param	bg		Color to draw over in RGB565 (16bit)
     * @param	alpha	Alpha in range 0-255
     **/
    uint16_t alphaBlendRGB565(uint32_t fg, uint32_t bg, uint8_t alpha)
        __attribute__((always_inline)) {
        alpha = (alpha + 4) >> 3; // from 0-255 to 0-31
        bg = (bg | (bg << 16)) & 0b00000111111000001111100000011111;
        fg = (fg | (fg << 16)) & 0b00000111111000001111100000011111;
        uint32_t result = ((((fg - bg) * alpha) >> 5) + bg) & 0b00000111111000001111100000011111;
        return (uint16_t)((result >> 16) | result); // contract result
    }

    /**
     * Same as above, but fg and bg are premultiplied, and alpah is already in range 0-31
     */
    uint16_t alphaBlendRGB565Premultiplied(uint32_t fg, uint32_t bg, uint8_t alpha)
        __attribute__((always_inline)) {
        uint32_t result = ((((fg - bg) * alpha) >> 5) + bg) & 0b00000111111000001111100000011111;
        return (uint16_t)((result >> 16) | result); // contract result
    }

    void drawFontBits(bool opaque, uint32_t bits, uint32_t numbits, int32_t x, int32_t y, uint32_t repeat);
    void charBounds(char c, int16_t *x, int16_t *y,
                    int16_t *minx, int16_t *miny, int16_t *maxx, int16_t *maxy);
};
#endif // __cplusplus

#endif