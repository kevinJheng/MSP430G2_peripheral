#include "redled.h"
#include "../LaunchpadRev1_5/launchpad.h"

void redbutton(void)
{
    P1OUT ^= red_LED;
}
