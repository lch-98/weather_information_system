#ifndef FONT_H
#define FONT_H

#include <stdint.h>

typedef struct {
    uint16_t unicode;
    const uint8_t *bitmap; // 32 바이트
} HangulFont;

typedef struct {
    uint16_t unicode;
    const uint8_t *bitmap; // 16 바이트
} SignNumFont;

extern HangulFont hangul_fonts[];
extern SignNumFont signnum_fonts[];
int get_hangul_font_count();
int get_signnum_font_count();

#endif