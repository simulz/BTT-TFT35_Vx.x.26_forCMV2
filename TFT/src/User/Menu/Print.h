#ifndef _PRINT_H_
#define _PRINT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

typedef struct
{
  bool isPossiblePrinting;
  float remainFilatWeight;
  float totalUsedFilaWeight;
  float filaDensity;
} FILAMENTWEIGHT;

typedef struct
{
  bool repeatPrint;
  uint16_t checkFileOfSourceNum;
  char lastPrintFilePath[1024];
} REPRINT;

extern REPRINT reprint;

extern FILAMENTWEIGHT filaweight;

void getlastPrintFilePath(void);
void menuPrintFromSource(void);
void menuPrint(void);
void startPrint(void);
void checkFileOfSource(void);
void setInfoFileSource(void);

#ifdef __cplusplus
}
#endif

#endif
