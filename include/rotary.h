#ifndef ROTARY_H
#define ROTARY_H

#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>

#include <stdio.h>
#include <string.h>

#define ROTARY_A (1 << PC0)
#define ROTARY_B (1 << PB7)

void rotary_init(void);
int get_rotary_state(void); // 00->0, 01->1, 11->3, 10->4
void updateRotaryState(void);

int clockwise; // 0 = CCW, 1= CW
int currState;
int prevState;
int value;



#endif