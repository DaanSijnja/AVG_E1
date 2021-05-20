/*
 */

#include <avr/io.h>
#include "lib/servo.h"

int main(void)
{

    // Insert code
    init_servo();
    while(1)
    {
        servo1_set_percentage(100);
        servo2_set_percentage(100);
    }
    ;

    return 0;
}
