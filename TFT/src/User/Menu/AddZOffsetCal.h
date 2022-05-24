#ifndef _ADD_Z_OFFSET_CAL_H_
#define _ADD_Z_OFFSET_CAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "includes.h"

#define PROBING_POINT_X0 35 
#define PROBING_POINT_Y0 85
#define PROBING_POINT_X1 195
#define PROBING_POINT_Y1 245

typedef struct
{
    uint16_t rowN;
    uint16_t colN;
    short rowindex;
    short colindex;
    float add_z_value[MESH_GRID_MAX_POINTS_X][MESH_GRID_MAX_POINTS_Y];
    float max_add_z_value, min_add_z_value;
    bool isNeedRedraw;
} ADDZOFFSETCAL;

extern ADDZOFFSETCAL addZOffsetCal;

void menuAddZOffsetCal(void);

#ifdef __cplusplus
}
#endif

#endif
