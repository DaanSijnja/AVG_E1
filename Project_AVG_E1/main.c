/*
 */

#include <avr/io.h>
#include "h_bridge.h"
#include "ultrasoon.h"

int main(void)
{
    init_h_bridge();
    init_ultrasoon();
    // Insert code

    while(1)
    {
        if(distance(ultra_1_trigger) >= 10){
            h_bridge_set_percentage(BACKWARDS,45);
        }
        else
        {
           h_bridge_set_percentage(FORWARDS,15);

        }


    }
    ;

    return 0;
}
