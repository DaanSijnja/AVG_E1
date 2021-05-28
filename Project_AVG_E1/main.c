/*
 */

#include <avr/io.h>
#include "h_bridge.h"

int main(void)
{
    init_h_bridge();
    // Insert code

    while(1)
    {

        h_bridge_set_percentage(BACKWARDS,45);



    }
    ;

    return 0;
}
