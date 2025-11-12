#include "button.h"

int button1_flag = 0;
int button2_flag = 0;
int button3_flag = 0;

// Chống dội = cách lưu 3 lần trạng thái
int Key_reg_0 = NORMAL_STATE;
int Key_reg_1 = NORMAL_STATE;
int Key_reg_2 = NORMAL_STATE;

int Key_reg_4 = NORMAL_STATE;
int Key_reg_5 = NORMAL_STATE;
int Key_reg_6 = NORMAL_STATE;

int Key_reg_a = NORMAL_STATE;
int Key_reg_b = NORMAL_STATE;
int Key_reg_c = NORMAL_STATE;

// Lưu trạng thái ổn đinh cuối cùng
int Key_reg_3 = NORMAL_STATE;
int Key_reg_7 = NORMAL_STATE;
int Key_reg_d = NORMAL_STATE;

// đếm giờ khi ấn nút lâu
int TimerForKeyPress = 200;

int isButtonPressed(){
	if(button1_flag == 1){
		button1_flag = 0;
		return 1;
	}
	else if(button2_flag == 1){
		button2_flag = 0;
		return 2;
	}
	else if(button3_flag == 1){
		button3_flag = 0;
		return 3;
	}
	else return 0;
}

void getKeyInput(){
	Key_reg_0 = Key_reg_1;
	Key_reg_1 = Key_reg_2; // so sánh trạng thái 3 lần
	Key_reg_2 = HAL_GPIO_ReadPin(Button_1_GPIO_Port, Button_1_Pin);

	Key_reg_4 = Key_reg_5;
	Key_reg_5 = Key_reg_6; // so sánh trạng thái 3 lần
	Key_reg_6 = HAL_GPIO_ReadPin(Button_2_GPIO_Port, Button_2_Pin);

	Key_reg_a = Key_reg_b;
	Key_reg_b = Key_reg_c; // so sánh trạng thái 3 lần
	Key_reg_c = HAL_GPIO_ReadPin(Button_3_GPIO_Port, Button_3_Pin);

	if((Key_reg_0 == Key_reg_1) && (Key_reg_1 == Key_reg_2)){
		if(Key_reg_3 != Key_reg_2) {
			Key_reg_3 = Key_reg_2;
			if(Key_reg_2 == PRESSED_STATE){
				//TODO
				button1_flag = 1;
				TimerForKeyPress = 200;
			}
		}
		else{
			TimerForKeyPress--;
			if(TimerForKeyPress == 0) {
				// TODO
				if(Key_reg_2 == PRESSED_STATE)
				button1_flag = 1;
				TimerForKeyPress = 200;
			}

		}
	}

	if((Key_reg_4 == Key_reg_5) && (Key_reg_5 == Key_reg_6)){
		if(Key_reg_7 != Key_reg_6) {
			Key_reg_7 = Key_reg_6;
			if(Key_reg_6 == PRESSED_STATE){
				//TODO
				button2_flag = 1;
				TimerForKeyPress = 200;
			}
		}
		else{
			TimerForKeyPress--;
			if(TimerForKeyPress == 0) {
				// TODO
				if(Key_reg_6 == PRESSED_STATE)
				button2_flag = 1;
				TimerForKeyPress = 200;
			}

		}
	}

	if((Key_reg_a == Key_reg_b) && (Key_reg_b == Key_reg_c)){
		if(Key_reg_d != Key_reg_c) {
			Key_reg_d = Key_reg_c;
			if(Key_reg_c == PRESSED_STATE){
				//TODO
				button3_flag = 1;
				TimerForKeyPress = 200;
			}
		}
		else{
			TimerForKeyPress--;
			if(TimerForKeyPress == 0) {
				// TODO
				if(Key_reg_c == PRESSED_STATE)
				button3_flag = 1;
				TimerForKeyPress = 200;
			}

		}
	}
}

