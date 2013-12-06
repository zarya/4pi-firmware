#include <board.h>
#include <pio/pio.h>
#include <stdio.h>

#include "hdlcd.h"
#include "lcdpcf8574.h"

void hdlcd_setup(void)
{
    //init lcd
    lcd_init(LCD_DISP_ON_BLINK);

    //lcd go home
    lcd_home();

    lcd_gotoxy(0, 0);
    lcd_puts("4PI Electronics starting up ");
}
