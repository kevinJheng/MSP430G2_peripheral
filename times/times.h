#ifndef __TIMES_H
#define __TIMES_H true

typedef struct {
    unsigned char hour;
    unsigned char minute;
    unsigned char sec;
    unsigned char min_inc;
} TIMES;

// counts min_inc with carry_up_boundary
int times_step_up(TIMES * target,int carry_up_boundary);

#endif
