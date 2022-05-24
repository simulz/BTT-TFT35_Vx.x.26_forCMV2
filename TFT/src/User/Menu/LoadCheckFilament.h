#ifndef _LOAD_CHECK_FILAMENT_H_
#define _LOAD_CHECK_FILAMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
  uint16_t sgResultValue;
  uint16_t detectSgrValue;
  float avgDetectSgr;
  uint16_t numOfGroup;
} LOADCHECKFILAMENT;

extern LOADCHECKFILAMENT loadCheckFila;
extern bool isTestingFilaExt;

void initN();
float getSgrValue();
void increaseDetectSgrValue();
void setSgrValue(float num);
void menuLoadCheckFilament(void);

#ifdef __cplusplus
}
#endif

#endif
