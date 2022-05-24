#ifndef _INTERFACECMD_H_
#define _INTERFACECMD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "stdbool.h"

#define CMD_MAX_LIST 20
#define CMD_MAX_SIZE 100
#define NOSIE_FILTER_TIME_INTERVAL 200
#define NO_TFT_DISK false
#define TFT_DISK    true

typedef char CMD[CMD_MAX_SIZE];

typedef struct
{
  char gcode[CMD_MAX_SIZE];
  uint8_t src;   // 0: TouchScreen Cmd, 1: Serial Port 2 rx Cmd, 2: Serial Port 3 rx Cmd
  bool isfromTFTDisk;
}GCODE;

typedef struct
{
  GCODE   queue[CMD_MAX_LIST];
  uint8_t index_r; // Ring buffer read position
  uint8_t index_w; // Ring buffer write position
  uint8_t count;   // Count of commands in the queue
}GCODE_QUEUE;

extern uint8_t debugNum1;
extern uint8_t debugNum2;
extern uint16_t debugNum3;
extern uint8_t debugNum4;
extern uint8_t debugNum5;

extern char noise_filter_cmd[CMD_MAX_SIZE];
extern char debugCmdChar;

uint32_t sendCmdTime;

extern uint32_t sendCmdTime;
extern GCODE_QUEUE infoCmd;
extern GCODE_QUEUE infoCacheCmd;

bool isFullCmdQueue(void);      // condition callback for loopProcessToCondition()
bool isNotEmptyCmdQueue(void);  // condition callback for loopProcessToCondition()

bool storeCmd(const char * format,...);
void mustStoreCmd(const char * format,...);
void mustStoreScript(const char * format,...);
bool storeCmdFromUART(uint8_t port, const char * gcode, bool isfromTFTDisk);
void mustStoreCacheCmd(const char * format,...);
bool moveCacheToCmd(void);
void clearCmdQueue(void);
void parseQueueCmd(void);
void sendQueueCmd(void);

#ifdef __cplusplus
}
#endif

#endif

