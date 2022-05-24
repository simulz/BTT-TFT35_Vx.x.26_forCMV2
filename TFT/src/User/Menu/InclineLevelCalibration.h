#ifndef _INCLINE_LEVEL_CALIBARTION_H_
#define _INCLINE_LEVEL_CALIBARTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

#define BED_RECT_X0  90
#define BED_RECT_Y0  90
#define BED_RECT_X1  260
#define BED_RECT_Y1  260

typedef struct
{
    float rf_z_value;
    float rb_z_value;
    float lf_z_value;
    float lb_z_value;
    float avg_z_value;
} INCLINELEVELCALi;

extern INCLINELEVELCALi inclineLevelCali;
extern bool isChangedValue;

void menuInclineLevelCali(void);
void drawBedValue();

#ifdef __cplusplus
}
#endif

#endif