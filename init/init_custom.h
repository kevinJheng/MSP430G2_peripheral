#ifndef __INIT_CUSTOM_H
#define __INIT_CUSTOM_H

void Init_Timer(void);
void start_up_INIT(void);
void Init_GPIO(void);
void Init_clock_system(void);
void Init_ADC10(void);
void Init_ADC10_DTC_trigCCR0_Single_Channel(unsigned char sizDTC,unsigned * stAddr);


#endif
