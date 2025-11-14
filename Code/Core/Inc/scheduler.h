#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>
#include "main.h"

typedef struct{
	void (*pTask)(void);
	uint32_t Delay;
	uint32_t Period;
	uint8_t  RunMe; // == Timerflag

	uint32_t TaskID;
}sTasks;

#define SCH_MAX_TASKS 40
#define ERROR_SCH_CANNOT_DELETE_TASK 99
#define ERROR_SCH_TOO_MANY_TASKS 88
#define RETURN_NORMAL 0
#define RETURN_ERROR 1

extern uint8_t Error_code_G;

typedef uint8_t tByte;

void SCH_Init(void);

// Giống setTimer - trả về ID của task (index trong mảng)
uint32_t SCH_Add_Task (void (*pFunction)(), uint32_t DELAY, uint32_t PERIOD, uint32_t existingID);

// Giống TimerRun();
void SCH_Update(void);

// Kiểm tra flag
void SCH_Dispatch_Tasks(void);

uint8_t SCH_Delete_Task(uint32_t ID);

void SCH_Report_Status(void);
void SCH_Go_To_Sleep(void);

#endif /* INC_SCHEDULER_H_ */
