# SSD1306.c - OLED Display Driver for AVR

This file has code for using SSD1306 OLED display with AVR microcontroller.  
It uses SPI to talk with display.  

Includes:  

- SPI setup and send data  
- Commands to start and control display  
- Set position, clear screen, draw letters and words  
- 5x7 font saved in program memory  
- Supports numbers, big and small letters  

Works with AVR chips like ATmega328P with any CPU speed (`F_CPU`).  

You can use these functions alone or in small library for easy OLED use.
