#ifndef __PWM_H__
#define __PWM_H__


int pwm_ctrl(uint32_t ch, uint32_t ctrl, uint32_t param);
int pwm_init(uint32_t ch, uint32_t freq);
int pwm_deinit(uint32_t ch);

#endif //__PWM_H__
