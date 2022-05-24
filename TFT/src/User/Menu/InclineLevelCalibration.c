#include "includes.h"
#include "InclineLevelCalibration.h"

INCLINELEVELCALi inclineLevelCali;

const MENUITEMS inclineLevelCaliItems = {
  // title
  LABEL_BED_LEVELING,
  // icon                         label
  {{ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_RESUME,                  LABEL_TEST},
   {ICON_BACK,                    LABEL_BACK},}
};

char tempstr[40];
GUI_RECT bedRecT = {BED_RECT_X0, BED_RECT_Y0, BED_RECT_X1, BED_RECT_Y1};

bool isChangedValue = false;

void drawBedInDisplay(void)
{
  GUI_FillRectColor(bedRecT.x0, bedRecT.y0, bedRecT.x1, bedRecT.y1, GRAY);  

  GUI_SetColor(CREMAKERORANGE);

  GUI_FillCircle(bedRecT.x0 + 4, bedRecT.y0 + 4, 7);
  GUI_FillCircle(bedRecT.x0 + 4, bedRecT.y1 - 4, 7);
  GUI_FillCircle(bedRecT.x1 - 4, bedRecT.y1 - 4, 7);
  GUI_FillCircle(bedRecT.x1 - 4, bedRecT.y0 + 4, 7);

  GUI_SetColor(WHITE);
}

void drawBedValue(void)
{
  GUI_ClearPrect(&ss_screen_without_toast);
  menuDrawPage(&inclineLevelCaliItems);
  drawBedInDisplay();

  sprintf(tempstr, "%.3f", inclineLevelCali.rf_z_value);
  GUI_DispStringCenter(bedRecT.x1 + 10, bedRecT.y1 + 6, (uint8_t *)tempstr);

  sprintf(tempstr, "%.3f", inclineLevelCali.rb_z_value);
  GUI_DispStringCenter(bedRecT.x1 + 10, bedRecT.y0 - 30, (uint8_t *)tempstr);

  sprintf(tempstr, "%.3f", inclineLevelCali.lf_z_value);
  GUI_DispStringCenter(bedRecT.x0 - 10, bedRecT.y1 + 6, (uint8_t *)tempstr);

  sprintf(tempstr, "%.3f", inclineLevelCali.lb_z_value);
  GUI_DispStringCenter(bedRecT.x0 - 10, bedRecT.y0 - 30, (uint8_t *)tempstr);

  GUI_SetColor(BLACK);
  GUI_SetBkColor(GRAY);

  sprintf(tempstr, "평균 %.3f", inclineLevelCali.avg_z_value);
  GUI_DispStringCenter(bedRecT.x0 + (bedRecT.x1 - bedRecT.x0) / 2, bedRecT.y0 + (bedRecT.y1 - bedRecT.y0) / 2, (uint8_t *)tempstr);

  GUI_SetColor(WHITE);
  GUI_SetBkColor(BLACK);
}

void drawTextHoriLv(void)
{
  sprintf(tempstr, "X 수평 체크 중입니다.");
  GUI_DispString(TEXT_LINE_X_VALUE, SECONE_LINE_Y_VALUE, (uint8_t *)tempstr);

  sprintf(tempstr, "잠시 기다려주세요.");
  GUI_DispString(TEXT_LINE_X_VALUE, THIRD_LINE_Y_VALUE, (uint8_t *)tempstr);
}

void menuInclineLevelCali(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&inclineLevelCaliItems);  
  drawTextHoriLv();

  while (infoMenu.menu[infoMenu.cur] == menuInclineLevelCali)
  {
    key_num = menuKeyGetValue();

    if(infoTesting.progress == ONE_PROGRESS_NUM)
    {
      GUI_ClearPrect(&ss_screen_without_toast);
      storeCmd("M101 Z\n");
      menuDrawPage(&inclineLevelCaliItems);
      drawBedInDisplay();
      plusTestingProgress();
    }
    else if(infoTesting.progress > ONE_PROGRESS_NUM)
    {
      if(isChangedValue)
      {
        drawBedValue();
        isChangedValue = false;
      } 
    }

    switch (key_num)
    {
      case KEY_ICON_0:
        //left front side
        storeCmd("M101 H0\n");
        break;

      case KEY_ICON_1:
        //right front side
        storeCmd("M101 H1\n");
        break;

      case KEY_ICON_2:
        //right back side
        storeCmd("M101 H2\n");
        break;

      case KEY_ICON_3:
        //left back side
        storeCmd("M101 H3\n");
        break;

      case KEY_ICON_6:
        //test all side
        infoTesting.progress = ONE_PROGRESS_NUM;
        break;

      case KEY_ICON_7:
        setZeroTestingProgress();
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}