#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "ds1631.h"

int main(void)
{
    ds1631_init();
    unsigned char buf[1] = {DS1631_CONFIG_1SHOT};
    ds1631_set_config(buf);
    ds1631_start_convert();
    int temp = ds1631_read_temp();
}
