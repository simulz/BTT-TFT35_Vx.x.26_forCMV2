#ifndef _PROBE_TEMP_CALIBRATION_H_
#define _PROBE_TEMP_CALIBRATION_H_

#ifdef __cplusplus
extern "C" {
#endif

#define SEASON_SUMMER 0
#define SEASON_WINTER 1

extern uint16_t seasonValue;
extern bool isTestingTempCal;
extern const GUI_RECT screen_without_Right;

void menuProbeTempCalibration(void);

#ifdef __cplusplus
}
#endif


#endif