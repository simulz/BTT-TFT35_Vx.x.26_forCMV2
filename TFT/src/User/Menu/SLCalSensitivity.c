#include "SLCalSensitivity.h"
#include "includes.h"

char tempstr[20];
uint16_t xSensitivity = 0;
uint16_t ySensitivity = 0;

uint16_t originXValue = 0;
uint16_t originYValue = 0;

bool isItX = true;
bool doNotDraw = false;

const MENUITEMS SLCalSensitivity = {
  // title
  LABEL_SL_CAL_SENSITIVITY,
  // icon                         label
  {{ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_RESET_VALUE,             LABEL_RESET},
   {ICON_EEPROM_SAVE,             LABEL_SAVE},
   {ICON_TEST_SENSITIVITY,        LABEL_TEST_SENSITIVITY},
   {ICON_X,                       LABEL_BACKGROUND},
   {ICON_Y,                       LABEL_BACKGROUND},
   {ICON_BACK,                    LABEL_BACK},}
};

void drawXYSensitivityValue()
{
  if(doNotDraw) return;

  if(isItX)
  {
    GUI_SetColor(WHITE);
  }
  else
  {
    GUI_SetColor(GRAY);
  }   
  sprintf(tempstr, "X 감도 : %d  " , xSensitivity);
  GUI_DispString(40, 65, (uint8_t *)tempstr);

  if(isItX)
  {
    GUI_SetColor(GRAY);
  }
  else
  {
    GUI_SetColor(WHITE);
  }
  sprintf(tempstr, "Y 감도 : %d  " , ySensitivity);
  GUI_DispString(40, 115, (uint8_t *)tempstr);

  GUI_SetColor(WHITE);
}

void menuSLCalSensitivity(void)
{
  doNotDraw = false;

  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&SLCalSensitivity);

  originXValue = xSensitivity = getParameter(P_BUMPSENSITIVITY,X_STEPPER);
  originYValue = ySensitivity = getParameter(P_BUMPSENSITIVITY,Y_STEPPER);

  while (infoMenu.menu[infoMenu.cur] == menuSLCalSensitivity)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_2:
        xSensitivity = originXValue;
        ySensitivity = originYValue;
        break;

      case KEY_ICON_3:
        if (infoMachineSettings.EEPROM == 1)
        {
          doNotDraw = true;

          sprintf(tempstr, "M914 X%d\n" , xSensitivity);
          storeCmd(tempstr);
          sprintf(tempstr, "M914 Y%d\n" , ySensitivity);
          storeCmd(tempstr);

          originXValue = xSensitivity;
          originYValue = ySensitivity;

          setDialogText(SLCalSensitivity.title.index, LABEL_SAVE_VALUE_AFTER_TEST, LABEL_YES, LABEL_NO);
          showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
        }
        infoMenu.cur--;
        break;

      case KEY_ICON_4:
        sprintf(tempstr, "M914 X%d\n" , xSensitivity);
        storeCmd(tempstr);
        sprintf(tempstr, "M914 Y%d\n" , ySensitivity);
        storeCmd(tempstr);
        storeCmd("G28 X Y\n");
        storeCmd("M84\n");
        break;

      case KEY_ICON_5:
        isItX = true;
        break;

      case KEY_ICON_6:
        isItX = false;
        break;
      
      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        #if LCD_ENCODER_SUPPORT
          if (encoderPosition)
          {
            if(isItX)
            {
                xSensitivity += encoderPosition;
            }
            else
            {
                ySensitivity += encoderPosition;
            }
            encoderPosition=0;
          }
        #endif
        break;
    }

    loopProcess();
    drawXYSensitivityValue();
    setParameter(P_BUMPSENSITIVITY, X_STEPPER, xSensitivity);
    setParameter(P_BUMPSENSITIVITY, Y_STEPPER, ySensitivity);
  }
}
