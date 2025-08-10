#ifndef SSD1306_H
#define SSD1306_H


#define SSD1306_WIDTH  128
#define SSD1306_HEIGHT 64
#define SSD1306_PAGES  (SSD1306_HEIGHT / 8)

#include <stdint.h>

static inline void spi_transfer(uint8_t b);
void spi_init(void);
void ssd1306_cmd(uint8_t c);
void ssd1306_set_pos(uint8_t col, uint8_t page);
void ssd1306_init(void);
void ssd1306_clear_buffer(void);
static uint8_t font_index_for_char(char ch);
void ssd1306_draw_char_buffer(uint8_t col, uint8_t page, char ch);
void ssd1306_display(void);
void ssd1306_draw_string_buffer(uint8_t col, uint8_t page, const char *s);
void ssd1306_scroll_text(const char* text, uint8_t page, uint8_t speed_ms, uint8_t repeats, uint8_t direction);
#endif
