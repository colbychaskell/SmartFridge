#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>

#include "ds1631.h"

int main(void)
{
    ds1631_init();
    int temp = ds1631_read_temp();

}
