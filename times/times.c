
#include "times.h"


// counts min_inc with carry_up_boundary
int times_step_up(TIMES * target,int carry_up_boundary)
{
    if(carry_up_boundary > 0)
	target->min_inc +=1;
    else 
	target->min_inc = carry_up_boundary; //for don't use min_minc case
    	

    if (target->min_inc == carry_up_boundary){ //min_inc handler
	target->min_inc =0; target->sec +=1;

	if (target->sec == 60) {//sec handler
	    target->sec =0;  target->minute+=1;

	    if (target->minute == 60) {//minute handler
		target->minute =0;  target->hour+=1;

		if (target->hour == 24) {
		    target->hour =0;  return 1;//return after 1 day
		}
	    }
	}
    }
    return 0;
}
