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
        servoleft_set_percentage(-95.4);
        servoright_set_percentage(100);
    }
    ;

    return 0;
}
