#define F_CPU 16000000UL //Define your XCLK

#include <avr/io.h>
#include <util/delay.h>
#include "../ssd1306.h"

int main(void) {
    spi_init();
    ssd1306_init();
    ssd1306_clear();
    
    ssd1306_draw_string(0, 0, " Hello World!");
    
    while (1) {

    }
    return 0;
}
