#include "LoadCheckFilament.h"
#include "includes.h"

LOADCHECKFILAMENT loadCheckFila;
bool isTestingFilaExt = false;

bool firstTestLoacCheckFila = false;

uint16_t oldDetectSgrValue;
uint16_t oldSgResultValue;
uint16_t oldAvgDetectSgr;

const MENUITEMS loadcheckfilament = {
  // title
  LABEL_LOAD_CHECK_FILAMENT,
  // icon                         label
  {{ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_REPEAT,                  LABEL_NEXT},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACK,                    LABEL_BACK},}
};

float calAvg(float currentAvg, uint16_t currentValue)
{
  loadCheckFila.numOfGroup++;
  return ((currentAvg * (loadCheckFila.numOfGroup-1)) + currentValue)/(loadCheckFila.numOfGroup); //calculate a next average value
}

void initN()
{
    loadCheckFila.detectSgrValue = 0;
    loadCheckFila.sgResultValue  = 0;
    loadCheckFila.avgDetectSgr   = 0;
    loadCheckFila.numOfGroup     = 1;
}

void setSgrValue(float num)
{
    loadCheckFila.sgResultValue = num;
    loadCheckFila.avgDetectSgr = calAvg(loadCheckFila.avgDetectSgr,loadCheckFila.sgResultValue);
}

float getSgrValue()
{
    return loadCheckFila.sgResultValue;
}

void increaseDetectSgrValue()
{
    loadCheckFila.detectSgrValue++;
}

void drawSgrValue()
{
    setLargeFont(true);

    char tempstr[10];
    char sgrStr[20] = "SGR : ";
    char sgrAvgValue[20] = "SGR 평균 :";
    char sgrDetectStr[20] = "감지된 횟수 : ";

    if(loadCheckFila.sgResultValue != oldSgResultValue || !firstTestLoacCheckFila)
    {
      GUI_DispString(80, 100, (uint8_t*) sgrStr);    
      sprintf(tempstr, "%d", loadCheckFila.sgResultValue);
      GUI_DispString(260, 100, (uint8_t*) tempstr);  

      oldSgResultValue = loadCheckFila.sgResultValue;  
    }

    if(loadCheckFila.avgDetectSgr != oldAvgDetectSgr || !firstTestLoacCheckFila)
    {
      GUI_DispString(80, 160, (uint8_t*) sgrAvgValue);
      sprintf(tempstr, "%.2f", loadCheckFila.avgDetectSgr);
      GUI_DispString(260, 160, (uint8_t*) tempstr);  

      oldAvgDetectSgr =  loadCheckFila.avgDetectSgr;   
    }

    if(loadCheckFila.detectSgrValue != oldDetectSgrValue || !firstTestLoacCheckFila)
    {
      GUI_DispString(80, 220, (uint8_t*) sgrDetectStr);    
      sprintf(tempstr, "%d", loadCheckFila.detectSgrValue);
      GUI_DispString(260, 220, (uint8_t*) tempstr);

      oldDetectSgrValue = loadCheckFila.detectSgrValue;
    }

    firstTestLoacCheckFila = true;
    setLargeFont(false);
}

void menuLoadCheckFilament(void)
{
  oldDetectSgrValue = 0;
  oldAvgDetectSgr = 0;
  oldSgResultValue = 0;

  initN();
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&loadcheckfilament);
  while (infoMenu.menu[infoMenu.cur] == menuLoadCheckFilament)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_3:
      initN();
      storeCmd("M415\n");
      break;

    case KEY_ICON_7:
      storeCmd("M414\n");
      finishTesting();
      infoMenu.cur--;
      loadCheckFila.avgDetectSgr = 0;
      loadCheckFila.numOfGroup = 1;
      break;

    default:break;
    }
    loopProcess();
    
    if(isTestingFilaExt)
      drawSgrValue();
  }
}