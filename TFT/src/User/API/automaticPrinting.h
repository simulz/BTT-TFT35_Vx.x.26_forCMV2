#ifndef _AUTOMATIC_PRINTING_H_
#define _AUTOMATIC_PRINTING_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

extern bool rightOrLeft;   //true is Left, false if Right
extern bool isAutoPrinting; 
extern void setRightOrLeft(bool RnL);

#ifdef __cplusplus
}
#endif

#endif