#ifndef SSD1306_H
#define SSD1306_H

#include <stdint.h>

void spi_init(void);
void ssd1306_init(void);
void ssd1306_clear(void);
void ssd1306_draw_char(uint8_t col, uint8_t page, char ch);
void ssd1306_draw_string(uint8_t col, uint8_t page, const char *s);

#endif
