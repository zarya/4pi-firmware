/*
pcf8574 lib 0x02

copyright (c) Davide Gironi, 2012

Changed made for sam3u by Rudy Hardman (Zarya), 2013

Released under GPLv3.
Please refer to LICENSE file for licensing information.
*/

#include <board.h>
#include <pio/pio.h>
#include <stdio.h>
#include <twi/twi.h>
#include <drivers/twi/twid.h>
#include <stdint.h>

#include "pcf8574.h"

//------------------------------------------------------------------------------
//         Local variables
//------------------------------------------------------------------------------

#define TWCK            100000

/// Pio pins to configure.
static const Pin pins[] = {PINS_TWI};

/// TWI driver instance.
static Twid twid;

//------------------------------------------------------------------------------
///        Local functions
//------------------------------------------------------------------------------

/*
 * initialize
 */
void pcf8574_init() {
    AT91C_BASE_PMC->PMC_PCER = 1 << IROn_TWI0;
    PIO_Configure(pins, PIO_LISTSIZE(pins));
    TWI_ConfigureMaster(AT91C_BASE_TWI, TWCK, BOARD_MCK);
    TWID_Initialize(&twid, AT91C_BASE_TWI);

    //reset the pin status
    int i = 0;
    for(i=0; i<PCF8574_MAXDEVICES; i++)
        pcf8574_pinstatus[i] = 0;

}

/*
 * get output status
 */
int8_t pcf8574_getoutput(uint8_t deviceid) {
    int8_t data = -1;
    if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES)) {
        data = pcf8574_pinstatus[deviceid];
    }
    return data;
}

/*
 * get output pin status
 */
int8_t pcf8574_getoutputpin(uint8_t deviceid, uint8_t pin) {
    int8_t data = -1;
    if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES) && (pin >= 0 && pin < PCF8574_MAXPINS)) {
        data = pcf8574_pinstatus[deviceid];
        data = (data >> pin) & 0b00000001;
    }
    return data;
}

/*
 * set output pins
 */
int8_t pcf8574_setoutput(uint8_t deviceid, uint8_t data) {
    if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES)) {
        pcf8574_pinstatus[deviceid] = data;
        //i2c_start(((PCF8574_ADDRBASE+deviceid)<<1) | I2C_WRITE);
        //i2c_write(data);
        //i2c_stop();
        unsigned char* output = (unsigned char*)&data;
        TWID_Write(&twid, (PCF8574_ADDRBASE+deviceid), 0, 0, output, 1, 0);
        return 0;
    }
    return -1;
}

/*
 * set output pins, replace actual status of a device from pinstart for pinlength with data
 */
int8_t pcf8574_setoutputpins(uint8_t deviceid, uint8_t pinstart, uint8_t pinlength, int8_t data) {
    //example:
    //actual data is         0b01101110
    //want to change              ---
    //pinstart                    4
    //data                        101   (pinlength 3)
    //result                 0b01110110
    if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES) && (pinstart - pinlength + 1 >= 0 && pinstart - pinlength + 1 >= 0 && pinstart < PCF8574_MAXPINS && pinstart > 0 && pinlength > 0)) {
        uint8_t b = 0;
        b = pcf8574_pinstatus[deviceid];
        uint8_t mask = ((1 << pinlength) - 1) << (pinstart - pinlength + 1);
        data <<= (pinstart - pinlength + 1);
        data &= mask;
        b &= ~(mask);
        b |= data;
        pcf8574_pinstatus[deviceid] = b;
        //update device
        //i2c_start(((PCF8574_ADDRBASE+deviceid)<<1) | I2C_WRITE);
        //i2c_write(b);
        //i2c_stop();
        unsigned char* output = (unsigned char*)&b; 
        TWID_Write(&twid, (PCF8574_ADDRBASE+deviceid), 0, 0, output, 1, 0);
        return 0;
    }
    return -1;
}

/*
 * set output pin
 */
int8_t pcf8574_setoutputpin(uint8_t deviceid, uint8_t pin, uint8_t data) {
    if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES) && (pin >= 0 && pin < PCF8574_MAXPINS)) {
        uint8_t b = 0;
        b = pcf8574_pinstatus[deviceid];
        b = (data != 0) ? (b | (1 << pin)) : (b & ~(1 << pin));
        pcf8574_pinstatus[deviceid] = b;
        //update device
        //i2c_start(((PCF8574_ADDRBASE+deviceid)<<1) | I2C_WRITE);
        //i2c_write(b);
        //i2c_stop();
        unsigned char* output = (unsigned char*)&b;
        TWID_Write(&twid, (PCF8574_ADDRBASE+deviceid), 0, 0, output, 1, 0);
        return 0;
    }
    return -1;
}

/*
 * set output pin high
 */
int8_t pcf8574_setoutputpinhigh(uint8_t deviceid, uint8_t pin) {
    return pcf8574_setoutputpin(deviceid, pin, 1);
}

/*
 * set output pin low
 */
int8_t pcf8574_setoutputpinlow(uint8_t deviceid, uint8_t pin) {
    return pcf8574_setoutputpin(deviceid, pin, 0);
}


/*
 * get input data
 */
int8_t pcf8574_getinput(uint8_t deviceid) {
    int8_t data = -1;
    if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES)) {
        //i2c_start(((PCF8574_ADDRBASE+deviceid)<<1) | I2C_READ);
        //data = ~i2c_readNak();
        //i2c_stop();
        //Hier moet nog een read functie in.
    }
    return data;
}

/*
 * get input pin (up or low)
 */
int8_t pcf8574_getinputpin(uint8_t deviceid, uint8_t pin) {
    int8_t data = -1;
    if((deviceid >= 0 && deviceid < PCF8574_MAXDEVICES) && (pin >= 0 && pin < PCF8574_MAXPINS)) {
        data = pcf8574_getinput(deviceid);
        if(data != -1) {
            data = (data >> pin) & 0b00000001;
        }
    }
    return data;
}

