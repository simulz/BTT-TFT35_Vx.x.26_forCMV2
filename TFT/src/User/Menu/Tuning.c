#include "Tuning.h"
#include "includes.h"

TESTING infoTesting;

const MENUITEMS TuningItems = {
  // title
  LABEL_TUNING,
  // icon                         label
  {{ICON_PID,                     LABEL_PID},
   {ICON_TUNE_EXTRUDER,           LABEL_TUNE_EXTRUDER},
   {ICON_TEMP_CALIBRATION,        LABEL_PROBE_TEMP_CALIBRATION},
   {ICON_SL_CAL_SENSITIVITY,      LABEL_SL_CAL_SENSITIVITY},
   {ICON_LEVELING,                LABEL_LEVELING_ADD_Z},
   //{ICON_LIN_ADV_K,               LABEL_LIN_ADV_K},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACK,                    LABEL_BACK},}
};

void menuTuning(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&TuningItems);

  while (infoMenu.menu[infoMenu.cur] == menuTuning)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuPid;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuTuneExtruder;
        break;

      case KEY_ICON_2:                
        infoMenu.menu[++infoMenu.cur] = menuProbeTempCalibration;
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuSLCalSensitivity;
        break;

      case KEY_ICON_4:
        storeCmd("M101\n");
        infoMenu.menu[++infoMenu.cur] = menuAddZOffsetCal;
        break;

      case KEY_ICON_5:
        //infoMenu.menu[++infoMenu.cur] = menuLinearAdvancedK;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}
