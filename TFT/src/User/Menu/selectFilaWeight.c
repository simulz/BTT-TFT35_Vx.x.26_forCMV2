#include "selectFilaWeight.h"
#include "includes.h"

FILAMENTWEIGHT filaweight;

char setFilaWeightStr[]  ="필라멘트 무게 설정완료.";
char setFilaDensity[] ="필라멘트 밀도 설정완료.";
char askToSetValueFila[] = "\n무게 설정을 하시겠습니까?";
int weightNum = 1; //1 is 1kg, 3 is 3kg


void drawFilaWeightValue(void)
{
    char tempstrFilaWeight[50];
    sprintf(tempstrFilaWeight, "현재 필라멘트 량 : %.2fg", filaweight.remainFilatWeight);
    GUI_DispString(25,60,(uint8_t *)tempstrFilaWeight);
    sprintf(tempstrFilaWeight, "밀도 : %.2fg/cm^3", filaweight.filaDensity);
    GUI_DispString(25,110,(uint8_t *)tempstrFilaWeight);
}

void setFilaWeight()
{
  if(weightNum==1)
  {
    filaweight.remainFilatWeight = 1000.0f;
  }
  else if(weightNum==3)
  {
    filaweight.remainFilatWeight = 3000.0f;
  }  
  addToast(DIALOG_TYPE_INFO, setFilaWeightStr);
  drawFilaWeightValue();
}

void menuSelectFilaWeight(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS selectFilaWeight = {
    // title
    LABEL_SET_FILA_WEIGHT,
    // icon                         label
    {{ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_CUSTOM_WEIGHT,           LABEL_BACKGROUND},
     {ICON_FILAMENT_1KG,            LABEL_BACKGROUND},
     {ICON_FILAMENT_3KG,            LABEL_BACKGROUND},
     {ICON_FILAMENT_DENSITY,        LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;

  char titlestr[40];
  char tempstr[100];

  menuDrawPage(&selectFilaWeight);
  drawFilaWeightValue();

  while (infoMenu.menu[infoMenu.cur] == menuSelectFilaWeight)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_3:
      {        
        sprintf(titlestr, "Min:0g | Max:5000g");

        float oldWeight = filaweight.remainFilatWeight;
        float val = numPadInt((uint8_t *) titlestr, filaweight.remainFilatWeight, 0, false);

        menuDrawPage(&selectFilaWeight);        

        if(oldWeight != val)
        {
          filaweight.remainFilatWeight = val;
          addToast(DIALOG_TYPE_INFO, setFilaWeightStr);
           
        } 
        
        drawFilaWeightValue();      

        break;
      }

      case KEY_ICON_4:
        weightNum = 1;
        sprintf(tempstr, "1000g으로");
        strcat(tempstr,askToSetValueFila);
        setDialogText(LABEL_INFO, (uint8_t *)tempstr, LABEL_YES, LABEL_NO);
        showDialog(DIALOG_TYPE_QUESTION, setFilaWeight, NULL, NULL);         
        break;

      case KEY_ICON_5:
        weightNum = 3;
        sprintf(tempstr, "3000g으로");
        strcat(tempstr,askToSetValueFila);
        setDialogText(LABEL_INFO, (uint8_t *)tempstr, LABEL_YES, LABEL_NO);
        showDialog(DIALOG_TYPE_QUESTION, setFilaWeight, NULL, NULL);
        break;

      case KEY_ICON_6:
        sprintf(titlestr, "밀도 (g/cm^3)");

        float oldDensity = filaweight.filaDensity;
        float val = numPadFloat((uint8_t *) titlestr, filaweight.filaDensity, filaweight.filaDensity, false);

        menuDrawPage(&selectFilaWeight);

        if(oldDensity != val)
        {
          filaweight.filaDensity = val;
          addToast(DIALOG_TYPE_INFO, setFilaDensity);   
        }        
               
        drawFilaWeightValue();

        break;

      case KEY_ICON_7:
        sprintf(titlestr, "M408 W%.2f D%.2f\n", filaweight.remainFilatWeight, filaweight.filaDensity);
        storeCmd(titlestr);
        storeCmd("M500\n");
        infoMenu.cur--;
        break;

      default:
        break;
    }

    loopProcess();
  }
}