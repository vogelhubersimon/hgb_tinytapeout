// File: src/pix_color.h
// Description: Color definitions and utilities for pixel processor
// USed for showing colors the way the sould look in final 6 Bit RGB format
#ifndef PIX_COLOR_H
#define PIX_COLOR_H
#include <stdint.h>

typedef uint8_t RGB222Color; // only low 6 bits used

// Extract individual color components
#define COLOR_R(c) (((c) >> 4) & 0x3)
#define COLOR_G(c) (((c) >> 2) & 0x3)
#define COLOR_B(c) ((c)        & 0x3)

// Create a color from individual components
#define MAKE_COLOR(r, g, b) (((r) << 4) | ((g) << 2) | (b))

#endif // PIX_COLOR_H