#ifndef INC_GLOBAL_H_
#define INC_GLOBAL_H_

#include "software_timer.h"
#include "main.h"

//#define INIT 	 	0
//#define AUTO_RED 	1
//#define AUTO_GREEN	2
//#define AUTO_YELLOW 3
//
//#define MAN_RED 	11
//#define MAN_GREEN	12
//#define MAN_YELLOW	13

#define INIT		0
#define AUTO_R1_G2	21
#define AUTO_R1_Y2	22
#define AUTO_G1_R2 	23
#define AUTO_Y1_R2	24

#define MODE_1		1
#define MODE_2		2
#define MODE_3		3
#define MODE_4		4

extern int status;
extern int mode;

#endif /* INC_GLOBAL_H_ */

