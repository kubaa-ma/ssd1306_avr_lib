#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "ssd1306.h"
#include<string.h>

int main(void) {
    spi_init();
    ssd1306_init();

    ssd1306_clear_buffer();
    ssd1306_display();
    while (1) {
        ssd1306_draw_string_buffer(1, 1, " Hello World");
        ssd1306_display();
        _delay_ms(3000);
        ssd1306_clear_buffer();
    }
}
