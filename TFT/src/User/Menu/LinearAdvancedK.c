#include "LinearAdvancedK.h"
#include "includes.h"

float valueK;
float oldValueK;

float xPosInLAK = 50;
float yPosInLAK = 40;
float extMM;
float extMMPerMove = 0.04;

short frValueSlow = 1200;
short frValueFast = 4800;
short frMove = 6000;
short frRetract = 3000;

short fastLength = 80;
short slowLength = 20;

short spaceOfLine = 15;

int startTime = 0;

bool isRecValueK = false;
bool didLeveling = false;

char adjustMsg[50] = "조정 중입니다.";

char valueKStr[10];
char tempMsg[120];

const MENUITEMS linearAdvancedK = {
  // title
  LABEL_LIN_ADVANCE,
  // icon                         label
  {{ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_RESUME,                  LABEL_START},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_EEPROM_SAVE,             LABEL_SAVE},
   {ICON_BACK,                    LABEL_BACK},}
};

void setOldValueK()
{
  storeCmd("M900 K%f\n", oldValueK); 
}

void setNewValueK()
{
  storeCmd("M900 K%f\n", valueK);
}

void menuLinearAdvancedK(void)
{
  startTime = OS_GetTimeMs();
  storeCmd("M900\n");

  while(!isRecValueK)
  {    
    loopProcess();

    if((OS_GetTimeMs() - startTime) > 5000)
    {
      break;
    }
  }
  
  if(isRecValueK)
    oldValueK = valueK;

  didLeveling = false;
  xPosInLAK = 50;
  yPosInLAK = 40;

  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&linearAdvancedK);
  while (infoMenu.menu[infoMenu.cur] == menuLinearAdvancedK)
  {
    sprintf(valueKStr, "K 값 : %.3f", valueK);
    GUI_DispString(40, 80, (uint8_t *)valueKStr);
    
    sprintf(valueKStr, "노즐 : %4d°C", heatGetCurrentTemp(heatGetCurrentHotend()));   
    GUI_DispString(40, 108, (uint8_t *)valueKStr);

    sprintf(valueKStr, "베드 : %4d°C", heatGetCurrentTemp(BED));   
    GUI_DispString(40, 132, (uint8_t *)valueKStr);

    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_3:
      if(!(heatGetCurrentTemp(heatGetCurrentHotend()) > 198 ) || !(heatGetCurrentTemp(BED) > 58))
      {
        sprintf(tempMsg, "온도가 낮습니다.\n예열을 시작하겠습니다.\n잠시 후 다시 실행해주세요.");
        setDialogText(LABEL_WARNING, (uint8_t *)tempMsg, LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_ERROR, NULL, NULL, NULL);
        heatSetTargetTemp(heatGetCurrentHotend(),200);
        heatSetTargetTemp(BED,60);
        break;
      }

      if(!didLeveling)
      {
        storeCmd("G28\n");
        storeCmd("G29\n");   
        storeCmd("M83\n");
        storeCmd("G92 E0\n");
      }   

      storeCmd("M900 K%f\n", valueK);   

      storeCmd("G0 X10 Y10 F6000\n");     
      extMM = 100*extMMPerMove;
      storeCmd("G1 E1 F\n", frRetract);
      storeCmd("G1 Y110 E%.5f F%d\n", extMM, frValueSlow);

      storeCmd("G1 E-1 F\n", frRetract);
      storeCmd("G0 X%.2f Y%.2f Z0.3 F%d\n", xPosInLAK, yPosInLAK, frMove);     

      xPosInLAK += slowLength; // xPosInLAK's value is 70      
      extMM = slowLength*extMMPerMove; // extMM's value is 0.8
      storeCmd("G1 E1 F\n", frRetract);
      storeCmd("G1 X%.2f Y%.2f E%.5f F%d\n", xPosInLAK, yPosInLAK, extMM, frValueSlow);

      xPosInLAK += fastLength; // xPosInLAK's value is 150
      extMM = fastLength*extMMPerMove; // extMM's value is 3.2
      storeCmd("G1 X%.2f Y%.2f E%.5f F%d\n", xPosInLAK, yPosInLAK, extMM, frValueFast);

      xPosInLAK += slowLength; // xPosInLAK's value is 170
      extMM = slowLength*extMMPerMove; // extMM's value is 0.8
      storeCmd("G1 X%.2f Y%.2f E%.5f F%d\n", xPosInLAK, yPosInLAK, extMM, frValueSlow);
      storeCmd("G1 E-1 F\n", frRetract);

      xPosInLAK = 50;
      yPosInLAK += spaceOfLine;
         
      if(yPosInLAK == 190)
      {
        yPosInLAK = 40;
      }
      else
      {
        storeCmd("G0 X200\n");
      }
      break;

    case KEY_ICON_6:
      if(didLeveling)
      {
        setDialogText(linearAdvancedK.title.index, LABEL_SAVE_VALUE_AFTER_TEST, LABEL_YES, LABEL_NO);
        showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
      }
      else
      {
        addToast(DIALOG_TYPE_INFO, adjustMsg);
      }
      break;

    case KEY_ICON_7:
      sprintf(tempMsg, "K 값을 저장할까요?");
      setDialogText(LABEL_WARNING, (uint8_t *)tempMsg, LABEL_YES, LABEL_NO);
      showDialog(DIALOG_TYPE_ERROR, extrusionMinTemp_OK, setOldValueK, NULL);

      saveEepromSettings();
      infoMenu.cur--;
      break;

    default:
      #if LCD_ENCODER_SUPPORT
        if (encoderPosition)
        {            
            valueK += (encoderPosition * 0.001);
            encoderPosition = 0;
        }
      #endif
      break;
    }

    loopProcess();
  }
}