#include <board.h>
#include <stdio.h>

#include "hdlcd.h"
#include "lcdpcf8574.h"
#include "gcode_parser.h"
#include "heaters.h"
#include "planner.h"

void hdlcd_setup(void)
{
    //init lcd
    lcd_init(LCD_DISP_ON);
    lcd_clrscr();
    lcd_led(0);
    lcd_puts("4Pi Starting up");
}

void hdlcd_main() {

    lcd_clrscr();
    lcd_home();

    heater_struct* heater = get_heater(0);

    char str[20];
    sprintf(str, "T1: %d\xdf", heater->akt_temp);
    lcd_puts(str);

    lcd_puts(" ");

    heater = get_heater(2);
    sprintf(str, "B: %d\xdf", bed_heater.akt_temp);
    lcd_puts(str);

    return;

    lcd_puts("    ");
    
    sprintf(str, "X%f/Y%f/Z%f", current_position[0],current_position[1],current_position[2]);
    lcd_puts(str);

    //lcd go home
    //lcd_home();

    //lcd_puts("12345678901234567890");
    //lcd_gotoxy(2,0);
    //lcd_puts("12345678901234567890");
    //lcd_gotoxy(3,0);    
    //lcd_puts("12345678901234567890");
    //lcd_gotoxy(4,0);
    //lcd_puts("12345678901234567890");
}
