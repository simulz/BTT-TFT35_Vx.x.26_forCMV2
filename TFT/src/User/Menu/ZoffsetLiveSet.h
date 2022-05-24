#ifndef _Z_OFFSET_LIVE_SET_H_
#define _Z_OFFSET_LIVE_SET_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

#define FRIST_LINE_TO_DISPALY  1
#define SECOND_LINE_TO_DISPALY 2
#define THIRD_LINE_TO_DISPALY  3
#define FOURTH_LINE_TO_DISPALY 4
#define FTFTH_LINE_TO_DISPALY  5
#define SIXTH_LINE_TO_DISPALY  6

#define GAP_OF_LINE            8

#define FRIST_LINE_Y_VALUE   112
#define SECONE_LINE_Y_VALUE  FRIST_LINE_Y_VALUE  + GAP_OF_LINE + BYTE_HEIGHT
#define THIRD_LINE_Y_VALUE   SECONE_LINE_Y_VALUE + GAP_OF_LINE + BYTE_HEIGHT
#define FOURTH_LINE_Y_VALUE  THIRD_LINE_Y_VALUE  + GAP_OF_LINE + BYTE_HEIGHT
#define FTFTH_LINE_Y_VALUE   FOURTH_LINE_Y_VALUE + GAP_OF_LINE + BYTE_HEIGHT
#define SXITH_LINE_Y_VALUE   FTFTH_LINE_Y_VALUE  + GAP_OF_LINE + BYTE_HEIGHT

#define TEXT_LINE_X_VALUE    48

void menuZoffsetLiveSet(void);
void drawTextInDisplay(char *text, uint16_t lineNum);

#ifdef __cplusplus
}
#endif

#endif