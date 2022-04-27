#include "rotary.h"

void rotary_init(void)
{
    DDRC &= ~ROTARY_A;
    DDRB &= ~ROTARY_B;
    PORTC |= ROTARY_A;
    PORTB |= ROTARY_B;
    value = 0;
    currState = -1;
}

/*


(0) 00 <--- 10 (3)
    |       ^
    |       |
    v       |
(1) 01 ---> 11 (2)

CCW is positive (add to the value)


CW is negative (subtract from value)

*/

int get_rotary_state(void)
{
    // 00
    if (~(PINC & ROTARY_A) && ~(PINB & ROTARY_B))
    {
        return 0;
    }
    // 01
    else if (~(PINC & ROTARY_A) && (PINB & ROTARY_B))
    {

        return 1;
    }
    // 11
    else if ((PINC & ROTARY_A) && (PINB & ROTARY_B))
    {

        return 2;
    }
    // 10
    else
    {
        return 3;
    }

    // 10
}

void updateRotaryState(void)
{
    
    
    // 00
     if (currState == 0)
    {
        if (prevState == 3)
        {
            // CCW
            value = (value + 1) % 27;
        }
        else
        {
            // CW
            value = (value + 1) % 27;
        }
    }
    // 01
    else if (currState == 1)
    {
        if (prevState == 0)
        {
            // CCW
            value = (value + 1) % 27;
        }
        else
        {
            // CW
            value = (value + 1) % 27;
        }
        // debounce rotary encoder
    }
    // 11
    else if (currState == 2)
    {
        if (prevState == 1)
        {
            // CCW
            value = (value + 1) % 27;
        }
        else
        {
            // CW
            value = (value + 1) % 27;
        }
    }
    // 10
    else
    {

        if (prevState == 2)
        {
            // CCW
            value = (value + 1) % 27;
        }
        else
        {
            // CW
            value = (value + 1) % 27;
        }
    }
}
