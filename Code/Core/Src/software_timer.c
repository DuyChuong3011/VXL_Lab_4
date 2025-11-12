#include "software_timer.h"

 int TIMER_CYCLE = 10;
 int TIMER1_CYCLE = 100;

int timer1_cnt = 0;
int timer1_flag = 0;

int timer2_cnt = 0;
int timer2_flag = 0;

int timer3_cnt = 0;
int timer3_flag = 0;

void setTimer1(int duration){
	timer1_cnt = duration / TIMER_CYCLE;
	timer1_flag = 0;
}

void setTimer2(int duration){
	timer2_cnt = duration / TIMER_CYCLE;
	timer2_flag = 0;
}

void setTimer3(int duration){
	timer3_cnt = duration / TIMER1_CYCLE;
	timer3_flag = 0;
}

void timerRun(){
	if(timer1_cnt > 0){
		timer1_cnt--;
		if(timer1_cnt <= 0){
			timer1_flag = 1;
		}
	}

	if(timer2_cnt > 0){
		timer2_cnt--;
		if(timer2_cnt <= 0){
			timer2_flag = 1;
		}
	}

	if(timer3_cnt > 0){
		timer3_cnt--;
		if(timer3_cnt <= 0){
			timer3_flag = 1;
		}
	}
}

