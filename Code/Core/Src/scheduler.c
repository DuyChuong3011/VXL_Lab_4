#include "scheduler.h"
#include "main.h"
#include "stm32f1xx_hal.h"

sTasks SCH_tasks_G[SCH_MAX_TASKS];
// uint8_t current_index_task = 0;
uint8_t Error_code_G = 0; 		 // Biến global lưu mã lỗi
uint8_t Last_error_code_G = 0; 	 // Biến global lưu mã lỗi trước đó
uint32_t Error_tick_count_G = 0; // Biến global đếm tick báo lỗi

void SCH_Init(void){
	// Khởi tạo tất cả pTask về 0x0000 (tức là xóa task)
	for(int i=0; i<SCH_MAX_TASKS; i++){
		SCH_tasks_G[i].pTask = 0x0000;
		SCH_tasks_G[i].Delay = 0;
		SCH_tasks_G[i].Period = 0;
		SCH_tasks_G[i].RunMe = 0;
	}
	Error_code_G = 0;
	Last_error_code_G = 0;
	Error_tick_count_G = 0;
}

// Hàm xóa task
uint8_t SCH_Delete_Task(const uint32_t TASK_INDEX){
	uint8_t Return_code;

	if (TASK_INDEX >= SCH_MAX_TASKS) {
		Return_code = RETURN_ERROR;
	} else if(SCH_tasks_G[TASK_INDEX].pTask == 0) {
		Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
		Return_code = RETURN_ERROR;
	} else {
		SCH_tasks_G[TASK_INDEX].pTask = 0x0000;
		SCH_tasks_G[TASK_INDEX].Delay = 0;
		SCH_tasks_G[TASK_INDEX].Period = 0;
		SCH_tasks_G[TASK_INDEX].RunMe = 0;
		Return_code = RETURN_NORMAL;
	}

	return Return_code;
}

// Hàm SCH_Add_Task (Hoàn thiện theo logic tìm vị trí trống)
uint32_t SCH_Add_Task (void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	int index = 0;
	// Tìm vị trí trống (pTask == 0)
	while((SCH_tasks_G[index].pTask != 0x0000) && (index < SCH_MAX_TASKS)){
		index++;
	}

	if(index == SCH_MAX_TASKS){
		// Task list is full
		Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
		return SCH_MAX_TASKS; // Trả về mã lỗi/index không hợp lệ
	}

	// Thêm task
	SCH_tasks_G[index].pTask = pFunction;
	SCH_tasks_G[index].Delay = DELAY;
	SCH_tasks_G[index].Period = PERIOD;
	SCH_tasks_G[index].RunMe = 0;
	SCH_tasks_G[index].TaskID = index; // Gán ID chính là index

	return index; // Trả về ID (index)
}

// Hàm SCH_Update (Chỉnh sửa logic lặp và Delay/Period/RunMe)
void SCH_Update(void){
	for(int i=0; i< SCH_MAX_TASKS; i++){ // Lặp qua tất cả các vị trí
		if(SCH_tasks_G[i].pTask != 0x0000){ // Chỉ xử lý nếu có task
			if(SCH_tasks_G[i].Delay > 0){
				SCH_tasks_G[i].Delay--;
			}else{
				// Task đến hạn chạy
				SCH_tasks_G[i].RunMe += 1;

				if(SCH_tasks_G[i].Period > 0){
					// Task định kỳ: nạp lại Delay
					SCH_tasks_G[i].Delay = SCH_tasks_G[i].Period;
				} else {
					// Task One-shot (Period == 0), Delay vẫn giữ bằng 0
					// Nó sẽ bị xóa trong Dispatch
				}
			}
		}
	}
}

void SCH_Dispatch_Tasks(void){
	for(int i=0; i<SCH_MAX_TASKS; i++){
		if(SCH_tasks_G[i].pTask != 0x0000 && SCH_tasks_G[i].RunMe > 0){
			// Chạy task
			(*SCH_tasks_G[i].pTask)();

			// Giảm RunMe flag
			SCH_tasks_G[i].RunMe--;

			// Nếu là task one-shot (Period == 0) và đã chạy xong (RunMe đã về 0 hoặc là lần chạy cuối)
			if(SCH_tasks_G[i].Period == 0 && SCH_tasks_G[i].RunMe == 0){
				SCH_Delete_Task(i);
			}
		}
	}

	// Report system status
	SCH_Report_Status();

	// The scheduler enters idle mode at this point
	SCH_Go_To_Sleep();
}

// Hàm Report Status
void SCH_Report_Status(void) {
	#ifdef SCH_REPORT_ERRORS
	// Check for a new error code
	if(Error_code_G != Last_error_code_G) {
		// Error_port = 255 - Error_code_G; // Giả định Error_port không được định nghĩa
		Last_error_code_G = Error_code_G;
		if (Error_code_G != 0){
			Error_tick_count_G = 60000;
		} else {
			Error_tick_count_G = 0;
		}
	} else {
		if (Error_tick_count_G != 0){
			if (--Error_tick_count_G == 0) {
				Error_code_G = 0; // Reset error code
			}
		}
	}
	#endif
}

// Hàm Go To Sleep
void SCH_Go_To_Sleep () {
	// todo
}



