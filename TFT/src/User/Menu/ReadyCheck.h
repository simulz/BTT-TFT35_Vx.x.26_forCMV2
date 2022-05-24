#ifndef _READY_CHECK_H_
#define _READY_CHECK_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

#define START_PROGRESS_NUM  0
#define ONE_PROGRESS_NUM    1
#define TWO_PROGRESS_NUM    2
#define THREE_PROGRESS_NUM  3
#define FOUR_PROGRESS_NUM   4


typedef struct
{
  uint16_t  progress;
  bool      testing; // 1 means printing, 0 means idle
} TESTING;

extern TESTING infoTesting;


#include "includes.h"

  void startTesting(void);
  void finishTesting(void);
  void plusTestingProgress(void);
  void minusTestingProgress(void);
  uint16_t getTestingProgress(void);
  void setTestingProgress(uint16_t num);
  void setZeroTestingProgress();
  void menuReadyCheck(void);

#ifdef __cplusplus
}
#endif


#endif

