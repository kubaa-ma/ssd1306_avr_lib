#ifndef F_CPU
#warning "F_CPU not defined"
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdint.h>
#include "ssd1306.h"


static uint8_t oled_buffer[SSD1306_WIDTH * SSD1306_PAGES];

#define SSD1306_DC   PB0 
#define SSD1306_RST  PB1
#define SSD1306_CS   PB2

const uint8_t font5x7[][5] PROGMEM = {
    {0x00,0x00,0x00,0x00,0x00}, {0x3E,0x51,0x49,0x45,0x3E}, {0x00,0x42,0x7F,0x40,0x00},
    {0x42,0x61,0x51,0x49,0x46}, {0x21,0x41,0x45,0x4B,0x31}, {0x18,0x14,0x12,0x7F,0x10},
    {0x27,0x45,0x45,0x45,0x39}, {0x3C,0x4A,0x49,0x49,0x30}, {0x01,0x71,0x09,0x05,0x03},
    {0x36,0x49,0x49,0x49,0x36}, {0x06,0x49,0x49,0x29,0x1E}, {0x7E,0x09,0x09,0x09,0x7E},
    {0x7F,0x49,0x49,0x49,0x36}, {0x3E,0x41,0x41,0x41,0x22}, {0x7F,0x41,0x41,0x22,0x1C},
    {0x7F,0x49,0x49,0x49,0x41}, {0x7F,0x09,0x09,0x09,0x01}, {0x3E,0x41,0x49,0x49,0x7A},
    {0x7F,0x08,0x08,0x08,0x7F}, {0x00,0x41,0x7F,0x41,0x00}, {0x20,0x40,0x41,0x3F,0x01},
    {0x7F,0x08,0x14,0x22,0x41}, {0x7F,0x40,0x40,0x40,0x40}, {0x7F,0x02,0x0C,0x02,0x7F},
    {0x7F,0x04,0x08,0x10,0x7F}, {0x3E,0x41,0x41,0x41,0x3E}, {0x7F,0x09,0x09,0x09,0x06},
    {0x3E,0x41,0x51,0x21,0x5E}, {0x7F,0x09,0x19,0x29,0x46}, {0x46,0x49,0x49,0x49,0x31},
    {0x01,0x01,0x7F,0x01,0x01}, {0x3F,0x40,0x40,0x40,0x3F}, {0x1F,0x20,0x40,0x20,0x1F},
    {0x7F,0x20,0x18,0x20,0x7F}, {0x63,0x14,0x08,0x14,0x63}, {0x07,0x08,0x70,0x08,0x07},
    {0x61,0x51,0x49,0x45,0x43},
    {0x00,0x00,0x5F,0x00,0x00},
    {0x02,0x01,0x51,0x09,0x06},
    {0x08,0x14,0x22,0x41,0x00},
    {0x41,0x22,0x14,0x08,0x00},
    {0x00,0x40,0x30,0x00,0x00},
    {0x00,0x60,0x20,0x00,0x00},
};

static inline void spi_transfer(uint8_t b) {
    SPDR = b;
    while (!(SPSR & (1<<SPIF)));
}


void sleep_ssd1306(uint8_t speed_ms){
    for(int i = 0; i < speed_ms; i++) _delay_ms(1);
}


void spi_init(void) {
    DDRB |= (1<<PB3) | (1<<PB5) | (1<<SSD1306_DC) | (1<<SSD1306_RST) | (1<<SSD1306_CS);
    PORTB |= (1<<SSD1306_CS) | (1<<SSD1306_DC) | (1<<SSD1306_RST);
    SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPR0);
}

void ssd1306_cmd(uint8_t c) {
    PORTB &= ~(1<<SSD1306_DC);
    PORTB &= ~(1<<SSD1306_CS);
    spi_transfer(c);
    PORTB |= (1<<SSD1306_CS);
}

void ssd1306_set_pos(uint8_t col, uint8_t page) {
    ssd1306_cmd(0xB0 | (page & 0x07)); 
    ssd1306_cmd(0x00 | (col & 0x0F)); 
    ssd1306_cmd(0x10 | ((col >> 4) & 0x0F)); 
}

void ssd1306_init(void) {
    PORTB &= ~(1<<SSD1306_RST);
    _delay_ms(10);
    PORTB |= (1<<SSD1306_RST);
    _delay_ms(10);

    ssd1306_cmd(0xAE);
    ssd1306_cmd(0xD5); ssd1306_cmd(0x80);
    ssd1306_cmd(0xA8); ssd1306_cmd(0x3F);
    ssd1306_cmd(0xD3); ssd1306_cmd(0x00);
    ssd1306_cmd(0x40);
    ssd1306_cmd(0x8D); ssd1306_cmd(0x14);
    ssd1306_cmd(0x20); ssd1306_cmd(0x00);
    ssd1306_cmd(0xA1);
    ssd1306_cmd(0xC8);
    ssd1306_cmd(0xDA); ssd1306_cmd(0x12);
    ssd1306_cmd(0x81); ssd1306_cmd(0xCF);
    ssd1306_cmd(0xD9); ssd1306_cmd(0xF1);
    ssd1306_cmd(0xDB); ssd1306_cmd(0x40);
    ssd1306_cmd(0xA4);
    ssd1306_cmd(0xA6);
    ssd1306_cmd(0x2E);
    ssd1306_cmd(0xAF);
}

void ssd1306_clear_buffer(void) {
    memset(oled_buffer, 0x00, sizeof(oled_buffer));
}

static uint8_t font_index_for_char(char ch) {
    if (ch == ' ') return 0;
    if (ch >= '0' && ch <= '9') return 1 + (ch - '0');
    if (ch >= 'A' && ch <= 'Z') return 11 + (ch - 'A');
    if (ch >= 'a' && ch <= 'z') return 11 + (ch - 'a');
    if (ch == '!') return 37;
    if (ch == '?') return 38;
    if (ch == '<') return 39;
    if (ch == '>') return 40;
    if (ch == '.') return 41;
    if (ch == ',') return 42;
    return 0;
}


void ssd1306_draw_char_buffer(uint8_t col, uint8_t page, char ch) {
    uint8_t idx = font_index_for_char(ch);
    for (uint8_t i = 0; i < 5; i++) {
        uint8_t b = pgm_read_byte(&font5x7[idx][i]);
        if (col + i < SSD1306_WIDTH) {
            oled_buffer[page * SSD1306_WIDTH + col + i] = b;
        }
    }
    if (col + 5 < SSD1306_WIDTH) {
        oled_buffer[page * SSD1306_WIDTH + col + 5] = 0x00;
    }
}

void ssd1306_draw_string_buffer(uint8_t col, uint8_t page, const char *s) {
    while (*s) {
        ssd1306_draw_char_buffer(col, page, *s++);
        col += 6;
        if (col > 122) {
            col = 0;
            page++;
            if (page >= SSD1306_PAGES) return;
        }
    }
}

void ssd1306_display(void) {
    for (uint8_t page = 0; page < SSD1306_PAGES; page++) {
        ssd1306_set_pos(0, page);
        PORTB |= (1 << SSD1306_DC);
        PORTB &= ~(1 << SSD1306_CS);
        for (uint8_t col = 0; col < SSD1306_WIDTH; col++) {
            spi_transfer(oled_buffer[page * SSD1306_WIDTH + col]);
        }
        PORTB |= (1 << SSD1306_CS);
    }
}

void ssd1306_scroll_text(const char* text, uint8_t page, uint8_t speed_ms, uint8_t repeats, uint8_t direction) {
    uint8_t len = strlen(text);

    int16_t start_pos, end_pos, step;

    if (direction == 0) { 
        start_pos = -6 * len;
        end_pos   = SSD1306_WIDTH;
        step      = 1;
    } else { 
        start_pos = SSD1306_WIDTH;
        end_pos   = -6 * len;
        step      = -1;
    }

    for (uint8_t r = 0; r < repeats; r++) {
        int16_t pos = start_pos;

        while ((direction == 0 && pos <= end_pos) ||
               (direction != 0 && pos >= end_pos)) {

            ssd1306_clear_buffer();

            int16_t x = pos;
            for (uint8_t i = 0; i < len; i++) {
                ssd1306_draw_char_buffer(x, page, text[i]);
                x += 6; // 5 px znak + 1 px mezera
            }

            ssd1306_display();
            sleep_ssd1306(speed_ms);

            pos += step;
        }
    }
}
