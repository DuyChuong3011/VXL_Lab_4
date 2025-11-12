#include "fsm.h"
#include "software_timer.h"
#include "main.h"
#include "global.h"
#include "button.h"

// --- BIẾN CỤC BỘ TRONG FSM (Dùng static) ---
static int led_buffer[4] = {0, 0, 0, 0};
static int led1_time = 0;
static int led2_time = 0;
static int r_time = 15; // Thoi gian Do mac dinh
static int g_time = 12; // Thoi gian Xanh mac dinh
static int y_time= 3; // Thoi gian Vang mac dinh
static int idx = 0; // Bien quet, luon phien 0 -> 1 -> 2 -> 3

// Bien cho Mode 2, 3, 4
static int blink_state = 0;
static int mode2_init = 1; // Co khoi tao cho Mode 2
static int mode3_init = 1; // Co khoi tao cho Mode 3
static int mode4_init = 1; // Co khoi tao cho Mode 4
static int cnt=0; // Bien dem gia tri (0-99)
static int next_mode = MODE_2; // Bien tam de xac dinh mode tiep theo khi nhan Button 1

// --- CHỨC NĂNG HIỂN THỊ LED 7 ĐOẠN (1 Port Segment) ---

void display7SEG(int num) {
    char segNumber[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};
    for (int i = 0; i < 7; ++i) {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 << i, (segNumber[num] >> i) & 1);
    }
}

// Quet 4 LED 7 doan (dung EN0, EN1, EN2, EN3)
void update7SEG(int index) {
    // 1. Cap nhat Buffer dua tren MODE
    if(mode == MODE_1){
        // Mode 1: Hien thi thoi gian
        led_buffer[0] = led1_time/10;
        led_buffer[1] = led1_time%10;
        led_buffer[2] = led2_time/10;
        led_buffer[3] = led2_time%10;
    } else {
        // Mode 2, 3, 4: Hien thi MODE (LED 1 & 2) va CNT (LED 3 & 4)
        led_buffer[0] = 0; // LED 1: Luon 0
        led_buffer[1] = mode; // LED 2: Hien thi Mode (1-4)
        led_buffer[2] = cnt/10; // LED 3: Hang chuc cua gia tri
        led_buffer[3] = cnt%10; // LED 4: Hang don vi cua gia tri
    }

    // 2. TAT TAT CA EN
    for(int i=0; i< 4; i++)
        HAL_GPIO_WritePin(GPIOA, EN0_Pin << i, GPIO_PIN_SET); // EN la Active-low (SET -> Tat)

    // 3. HIEN THI DU LIEU
    display7SEG(led_buffer[index]);

    // 4. BAT EN cho LED hien tai
    HAL_GPIO_WritePin(GPIOA, EN0_Pin << index, GPIO_PIN_RESET); // RESET -> Bat
}

// Quet 4 LED luon phien
void Scan_7seg() {
    if(timer2_flag == 1){
        setTimer2(50);
        update7SEG(idx);
        idx = (idx + 1) % 4; // Luon phien 0 -> 1 -> 2 -> 3
    }
    else if(timer2_cnt == 0){
        // Khoi dong lan dau
        setTimer2(50);
        idx = 0;
        // Tat tat ca EN ban dau
        for(int i=0; i< 4; i++)
            HAL_GPIO_WritePin(GPIOA, EN0_Pin << i, GPIO_PIN_SET);
    }
}

// --- CHỨC NĂNG ĐÈN GIAO THÔNG ---

void setTrafficLED(int r1, int g1, int y1, int r2, int g2, int y2) {
    // Logic: PIN_SET la TAT, PIN_RESET la BAT (Active-low)
    HAL_GPIO_WritePin(GPIOA, RED_1_Pin, (r1 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, GREEN_1_Pin, (g1 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, YELLOW_1_Pin, (y1 ? GPIO_PIN_SET : GPIO_PIN_RESET));

    HAL_GPIO_WritePin(GPIOA, RED_2_Pin, (r2 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, GREEN_2_Pin, (g2 ? GPIO_PIN_SET : GPIO_PIN_RESET));
    HAL_GPIO_WritePin(GPIOA, YELLOW_2_Pin, (y2 ? GPIO_PIN_SET : GPIO_PIN_RESET));
}

// Ham chuyen trang thai
void enterState(int new_state, int t1, int t2,
                int r1, int g1, int y1, int r2, int g2, int y2) {
    status = new_state;
    led1_time = t1;
    led2_time = t2;
    setTrafficLED(r1, g1, y1, r2, g2, y2);
    setTimer1(1000); // 1 giay
}

// --- LOGIC NÚT NHẤN VÀ CHUYỂN MODE ---

void check_button() {
    int button_event = isButtonPressed();
    switch (button_event) {
        case 1:
            // Button 1: Chuyen MODE
            mode = next_mode;

            // Dat lai co khoi tao mode
            mode2_init = 1;
            mode3_init = 1;
            mode4_init = 1;

            // Reset CNT khi chuyen mode va thiet lap next_mode
            if (mode == MODE_1) {
                // Quay ve Mode 1: Reset Status FSM
                status = INIT;
                // Ap dung gia tri thoi gian da set
                y_time = r_time - g_time; // Tinh lai Vang
                next_mode = MODE_2;
            } else if (mode == MODE_2) {
                cnt = (r_time > 0) ? r_time : 1; // Khoi tao CNT tu r_time, toi thieu la 1
                next_mode = MODE_3;
            } else if (mode == MODE_3) {
                cnt = (y_time > 0) ? y_time : 1; // Khoi tao CNT tu y_time, toi thieu la 1
                next_mode = MODE_4;
            } else if (mode == MODE_4) {
                cnt = (g_time > 0) ? g_time : 1; // Khoi tao CNT tu g_time, toi thieu la 1
                next_mode = MODE_1;
            }

            break;
        case 2:
            // Button 2: Tang gia tri CNT trong Mode 2, 3, 4
            if (mode >= MODE_2 && mode <= MODE_4) {
                cnt++;
                if (cnt > 99) cnt = 1; // Gia tri tu 1-99
            }
            break;
        case 3:
            // Button 3: SET gia tri va thoat ve Mode 1
            if (mode == MODE_2) {
                r_time = cnt;
            } else if (mode == MODE_3) {
                y_time = cnt;
            } else if (mode == MODE_4) {
                g_time = cnt;
            }

            // --- Ràng buộc thời gian (r_time = g_time + y_time) ---
            if (mode == MODE_2) {
                // Set r_time -> Tinh lai y_time hoac g_time. Ta chon tinh lai g_time
                g_time = r_time - y_time;
                if (y_time < 1) {
                    y_time = 1;
                    r_time = g_time + 1; // Dam bao r_time luon >= g_time + 1
                }
            } else if (mode == MODE_3 || mode == MODE_4) {
                // Set y_time hoac g_time -> Tinh lai r_time
                r_time = g_time + y_time;
            }

            // Sau khi SET, tro ve Mode 1
            mode = MODE_1;
            next_mode = MODE_2;
            status = INIT; // Reset FSM

            break;
        default:
            break;
    }
}

int checkErrorTraffic(){
	return ((r_time <0) | (g_time <0) | (y_time <0) |
		(r_time >99) | (g_time >99) | (y_time >99) | (r_time != g_time+ y_time));
}

// --- LOGIC CÁC MODE ---

void normal_mode() {
	if(checkErrorTraffic()){
		r_time = 5;
		g_time = 3;
		y_time = 2;
	}
    if (timer1_flag) {
        // Giam thoi gian moi giay
        if (led1_time > 0) led1_time--;
        if (led2_time > 0) led2_time--;

        setTimer1(1000); // lap lai 1 giay
    }
    // Logic FSM den giao thong
    switch (status) {
    case INIT:
        enterState(AUTO_R1_G2, r_time, g_time,
                    GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET, // D1: Do
                    GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_SET); // D2: Xanh
        break;

    case AUTO_R1_G2:
        if (led2_time <= 0) {
            enterState(AUTO_R1_Y2, led1_time, y_time,
                    GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET, // D1: Do
                    GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET); // D2: Vang
        }
        break;

    case AUTO_R1_Y2:
        if (led2_time <= 0) {
            enterState(AUTO_G1_R2, g_time, r_time,
                    GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_SET, // D1: Xanh
                    GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET); // D2: Do
        }
        break;

    case AUTO_G1_R2:
        if (led1_time <= 0) {
            enterState(AUTO_Y1_R2, y_time, led2_time,
                    GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET, // D1: Vang
                    GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET); // D2: Do
        }
        break;

    case AUTO_Y1_R2:
        if (led1_time <= 0) {
            enterState(AUTO_R1_G2, r_time, g_time,
                    GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET, // D1: Do
                    GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_SET); // D2: Xanh
        }
        break;

    default:
        enterState(INIT, 0, 0,
                    GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET,
                    GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET);
        break;
    }
}

void modify_mode(int target_led, int *duration, int *init_flag) {
    if (*init_flag) {
        // Khoi tao che do: Tat het den
        setTrafficLED(GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET,
                      GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET);
        blink_state = 0;
        setTimer1(250); // Bat dau chu ky nhay 2Hz
        *init_flag = 0;
    }

    if (timer1_flag) {
        if (blink_state == 0) {
            // BAT den nhay (2Hz)
            if (target_led == RED_1_Pin) {
                HAL_GPIO_WritePin(GPIOA, RED_1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, RED_2_Pin, GPIO_PIN_RESET);
            } else if (target_led == YELLOW_1_Pin) {
                HAL_GPIO_WritePin(GPIOA, YELLOW_1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, YELLOW_2_Pin, GPIO_PIN_RESET);
            } else if (target_led == GREEN_1_Pin) {
                HAL_GPIO_WritePin(GPIOA, GREEN_1_Pin, GPIO_PIN_RESET);
                HAL_GPIO_WritePin(GPIOA, GREEN_2_Pin, GPIO_PIN_RESET);
            }
            blink_state = 1;
        } else {
            // TAT den nhay
            setTrafficLED(GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET,
                          GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_SET);
            blink_state = 0;
        }
        setTimer1(250); // Lap lai sau 250ms (Tong chu ky 500ms = 2Hz)
    }
}


void fsm_auto_2way_run() {
    check_button(); // Xu ly nut nhan truoc

    switch (mode) {
    case MODE_1:
        normal_mode();
        break;
    case MODE_2:
        // Chinh sua thoi gian Do (RED)
        modify_mode(RED_1_Pin, &r_time, &mode2_init);
        break;
    case MODE_3:
        // Chinh sua thoi gian Vang (YELLOW)
        modify_mode(YELLOW_1_Pin, &y_time, &mode3_init);
        break;
    case MODE_4:
        // Chinh sua thoi gian Xanh (GREEN)
        modify_mode(GREEN_1_Pin, &g_time, &mode4_init);
        break;
    default:
        // Fallback
        mode = MODE_1;
        normal_mode();
        break;
    }
    Scan_7seg();
}
