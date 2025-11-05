#include "scheduler.h"
#include "main.h"

sTasks SCH_tasks_G[SCH_MAX_TASKS];
uint8_t currentTaskID = 0;
uint8_t Error_code_G = 0;
uint8_t Last_error_code_G = 0;
uint32_t Error_tick_count_G = 0;

#define NO_TASK_ID 0

static uint32_t Get_New_Task_ID(void){
    currentTaskID++;
    if(currentTaskID == 0 || currentTaskID == NO_TASK_ID){
        currentTaskID++; // Đảm bảo ID hợp lệ
    }
    return currentTaskID;
}

void SCH_Init(void){
	for(int i=0; i<SCH_MAX_TASKS; i++){
		SCH_tasks_G[i].pTask = 0x0000;
		SCH_tasks_G[i].Delay = 0;
		SCH_tasks_G[i].Period = 0;
		SCH_tasks_G[i].RunMe = 0;
	}
	Error_code_G = 0;
	Last_error_code_G = 0;
	Error_tick_count_G = 0;
	currentTaskID=0;
}

// Hàm SCH_Add_Task (Hoàn thiện theo logic tìm vị trí trống)
uint32_t SCH_Add_Task (void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD){
	uint8_t newTaskIndex = 0;
	uint32_t sumDelay = 0;
	uint32_t newDelay = 0;
	uint32_t taskID;

	for(newTaskIndex=0; newTaskIndex <SCH_MAX_TASKS; newTaskIndex++){

		// Trường hợp 1: Chèn vào vị trí trống cuối cùng
		if(SCH_tasks_G[newTaskIndex].pTask == 0x0000){
			SCH_tasks_G[newTaskIndex] .Delay = DELAY - sumDelay;
			break;
		}

		sumDelay += SCH_tasks_G[newTaskIndex].Delay;

		// Trường hợp 2: Chèn vào giữa hai task đang hoạt động
		if(sumDelay > DELAY){
			newDelay = DELAY-(sumDelay-SCH_tasks_G[newTaskIndex].Delay);
			SCH_tasks_G[newTaskIndex].Delay = sumDelay -DELAY;

			for(uint8_t i = SCH_MAX_TASKS -1; i > newTaskIndex; i--){
				SCH_tasks_G[i] = SCH_tasks_G[i-1];
			}

			SCH_tasks_G[newTaskIndex].pTask = pFunction;
			SCH_tasks_G[newTaskIndex].Delay = newDelay;
			SCH_tasks_G[newTaskIndex].Period = PERIOD;
			SCH_tasks_G[newTaskIndex].RunMe = (newDelay == 0) ? 1 : 0;
			taskID = Get_New_Task_ID();
			SCH_tasks_G[newTaskIndex].TaskID = taskID;

			return taskID;
		}
	}

	// Nếu newTaskIndex < SCH_MAX_TASKS (chèn vào vị trí trống)
	if(newTaskIndex < SCH_MAX_TASKS){
		SCH_tasks_G[newTaskIndex].pTask = pFunction;
		SCH_tasks_G[newTaskIndex].Period = PERIOD;
		taskID = Get_New_Task_ID();
		SCH_tasks_G[newTaskIndex].TaskID = taskID;
		return taskID;
	}

	Error_code_G = ERROR_SCH_TOO_MANY_TASKS;
	return SCH_MAX_TASKS;
}

uint8_t SCH_Delete_Task(const uint32_t taskID){
	uint8_t Return_code = RETURN_ERROR;
	uint8_t taskIndex;
	uint8_t j;

	if (taskID == 0 || taskID == NO_TASK_ID) return RETURN_ERROR;

	for(taskIndex =0; taskIndex < SCH_MAX_TASKS; taskIndex++){
		if(SCH_tasks_G[taskIndex].TaskID == taskID){
			Return_code = RETURN_NORMAL;

			if(taskIndex < SCH_MAX_TASKS - 1 &&
				SCH_tasks_G[taskIndex+1].pTask != 0x0000){
				SCH_tasks_G[taskIndex+1].Delay += SCH_tasks_G[taskIndex].Delay;
			}

			for(j = taskIndex; j < SCH_MAX_TASKS-1; j++)
				SCH_tasks_G[j] = SCH_tasks_G[j+1];

			SCH_tasks_G[j].pTask = 0x0000;
			SCH_tasks_G[j].Period = 0;
			SCH_tasks_G[j].Delay = 0;
			SCH_tasks_G[j].RunMe = 0;
			SCH_tasks_G[j].TaskID = 0;

			return Return_code;
		}
	}

	Error_code_G = ERROR_SCH_CANNOT_DELETE_TASK;
	return Return_code;
}


void SCH_Update(void){
	// Chỉ thao tác trên task đầu tiên (SCH_tasks_G[0])
	if(SCH_tasks_G[0].pTask != 0x0000 && SCH_tasks_G[0].RunMe == 0){
		if(SCH_tasks_G[0].Delay > 0) SCH_tasks_G[0].Delay--;

		// Nếu Delay bằng 0, task sẵn sàng chạy
		if (SCH_tasks_G[0].Delay == 0) SCH_tasks_G[0].RunMe = 1;
	}
}

void SCH_Dispatch_Tasks(void){
	// Chi kiểm tra task đầu tiên
	if(SCH_tasks_G[0].RunMe > 0 && SCH_tasks_G[0].pTask != 0x0000){
		sTasks temtask = SCH_tasks_G[0];

		// 1. Chạy task
		(*temtask.pTask)();

		// 2. Giảm cờ RunMe
		SCH_tasks_G[0].RunMe--;

		// 3. Xóa task khỏi mảng
		SCH_Delete_Task(temtask.TaskID);

		// 4. Nếu là task định kỳ, thêm lại vào hàng đợi
		if (temtask.Period != 0)
			SCH_Add_Task(temtask.pTask, temtask.Period, temtask.Period);
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



