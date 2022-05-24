#include "includes.h"
#include "ProbeTempCalibration.h"

#define SEASON_TOGGLE_X 260
#define SEASON_TOGGLE_Y 235

const GUI_RECT screen_without_Right = {0, TOAST_HEIGHT, 360, LCD_HEIGHT};

uint16_t seasonValue = SEASON_WINTER;

TESTING infoTesting;

char tempstr[50];
char CalibratingMsg[50] = "조정 중입니다.";

const ITEM itemEEPROM[1] = {
  // icon               label
  {ICON_EEPROM_SAVE,    LABEL_SAVE},
};

void drawTextNotice()
{
  sprintf(tempstr, "조정중입니다. 기다려주세요.");
  GUI_DispString(40, 120, (uint8_t *)tempstr);

  sprintf(tempstr, "장시간 소요될 수 있습니다.");
  GUI_DispString(40, 152, (uint8_t *)tempstr);
}

void drawToggleBtnForSeason()
{
  ListItem_DisplayToggle(SEASON_TOGGLE_X,   SEASON_TOGGLE_Y,   ICONCHAR_TOGGLE_ON);

  char tempstr[50];
  sprintf(tempstr, "여름");
  GUI_DispString(SEASON_TOGGLE_X - 30,  SEASON_TOGGLE_Y + 30,   (uint8_t *)tempstr);
  sprintf(tempstr, "겨울");
  GUI_DispString(SEASON_TOGGLE_X + 30,  SEASON_TOGGLE_Y + 30,   (uint8_t *)tempstr);
}

void menuProbeTempCalibration(void)
{  
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS ProbeTempCalibration = {
    // title
    LABEL_PROBE_TEMP_CALIBRATION, 
    // icon                         label
    {{ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_RESUME,                  LABEL_START},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK},}
  };

  static uint32_t BeginTime = 0;
  static uint32_t CurTime   = 0;

  static uint32_t timeHour = 0;
  static uint32_t timeMin  = 0;
  static uint32_t timeSec  = 0;

  seasonValue = SEASON_WINTER;
  bool isSeasonSelected = false;

  infoTesting.progress = ONE_PROGRESS_NUM;  

  menuDrawPage(&ProbeTempCalibration);

  sprintf(tempstr, "준비 완료시");
  GUI_DispString(40, 120, (uint8_t *)tempstr);
  
  sprintf(tempstr, "시작 버튼을 누르세요.");
  GUI_DispString(40, 152, (uint8_t *)tempstr);

  drawToggleBtnForSeason();

  while (infoMenu.menu[infoMenu.cur] == menuProbeTempCalibration)
  {      
    CurTime = OS_GetTimeMs();

    timeHour = ((CurTime - BeginTime) / (1000 * 60)) / 60;  // hour unit
    timeMin = ((CurTime - BeginTime) / (1000 * 60)) % 60;   // min unit
    timeSec = ((CurTime - BeginTime) % (1000 * 60)) / 1000; // sec unit

    if(infoTesting.testing && (infoTesting.progress != ONE_PROGRESS_NUM))
    {
      if(infoTesting.progress != THREE_PROGRESS_NUM)
      {
        sprintf(tempstr, "%02d:%02d:%02d 경과", timeHour, timeMin, timeSec);
        GUI_DispString(100, 80, (uint8_t *)tempstr);
      }         

      sprintf(tempstr, "베드 온도 : %d  ", heatGetCurrentTemp(BED));
      GUI_DispString(40, 216, (uint8_t *)tempstr);
    }
    else if(infoTesting.progress == THREE_PROGRESS_NUM)
    {
      sprintf(tempstr, "%02d:%02d:%02d 테스트 완료", timeHour, timeMin, timeSec);
      GUI_DispString(70, 80, (uint8_t *)tempstr);

      sprintf(tempstr, "저장하여 완료합니다.");
      GUI_DispString(70, 120, (uint8_t *)tempstr);
    }

    sprintf(tempstr, "센서 온도 : %d  ", getProbeTemperature());
    GUI_DispString(40, 248, (uint8_t *)tempstr);

    KEY_VALUES key_num = KEY_IDLE;
    key_num = menuKeyGetValue();

    switch (key_num)
    {        
      case KEY_ICON_3:
        if(!isSeasonSelected)
        {
          storeCmd("M871 W1\n");
          BeginTime = OS_GetTimeMs();
        }

        if(infoTesting.progress < TWO_PROGRESS_NUM)
        {
          infoTesting.progress++;

          startTesting();
          drawTextNotice();
          storeCmd("G76\n");
          ProbeTempCalibration.items[KEY_ICON_3] = itemEEPROM[0];
          menuDrawItem(&ProbeTempCalibration.items[KEY_ICON_3], KEY_ICON_3);
        }
        else if(!infoTesting.testing && (infoTesting.progress != 0))
        {
          setDialogText(ProbeTempCalibration.title.index, LABEL_SAVE_VALUE_AFTER_TEST, LABEL_YES, LABEL_NO);
          showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
        }
        else if(infoTesting.testing && (infoTesting.progress != 0))
        {
          addToast(DIALOG_TYPE_INFO, CalibratingMsg);
        }      
        break;

      case KEY_ICON_6:
        if(seasonValue == SEASON_WINTER)
        {
          ListItem_DisplayToggle(SEASON_TOGGLE_X, SEASON_TOGGLE_Y, ICONCHAR_TOGGLE_OFF);
          seasonValue = SEASON_SUMMER;
          storeCmd("M871 W0\n");
          addToast(DIALOG_TYPE_INFO, "여름 설정");
        }
        else if(seasonValue == SEASON_SUMMER)
        {
          ListItem_DisplayToggle(SEASON_TOGGLE_X, SEASON_TOGGLE_Y, ICONCHAR_TOGGLE_ON);
          seasonValue = SEASON_WINTER;
          storeCmd("M871 W1\n");
          addToast(DIALOG_TYPE_INFO, "겨울 설정");
        }

        if(!isSeasonSelected)
        {
          isSeasonSelected = true;
        }
        break;

      case KEY_ICON_7:
        infoTesting.progress = 0;

        if(infoTesting.testing)
        {
          addToast(DIALOG_TYPE_INFO, CalibratingMsg);
        }
        else if(!infoTesting.testing)
        {
          finishTesting();
          infoMenu.cur--; 
        }        
        break;

      default:
        break;
    }
    loopProcess();
  }
}