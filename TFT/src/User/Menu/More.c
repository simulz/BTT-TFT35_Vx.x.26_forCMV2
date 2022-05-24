#include "includes.h"
#include "More.h"

#define EPSILONE_FLOAT 0.000001

char errorNotLay1[100] = "첫번째 레이어가 아닙니다.";

const MENUITEMS moreItems = {
  // title
  LABEL_MORE,
  // icon                         label
  {{ICON_HEAT,                    LABEL_HEAT},
   {ICON_FAN,                     LABEL_FAN},   
#ifdef LOAD_UNLOAD_M701_M702
   {ICON_EXTRUDE,                 LABEL_LOAD_UNLOAD},
#else
   {ICON_GCODE,                   LABEL_TERMINAL},
#endif
   {ICON_BABYSTEP,                LABEL_BABYSTEP},
   {ICON_GCODE,                   LABEL_TERMINAL},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},

   {ICON_BACK,                    LABEL_BACK},}
};

void isPauseExtrude(void)
{
  if(setPrintPause(true,false))
    infoMenu.menu[infoMenu.cur] = menuExtrude;
}

void isPauseLoadUnload(void)
{
  storeCmd("M500\n");
  if(setPrintPause(!isPause(),false))
    infoMenu.menu[infoMenu.cur] = menuLoadUnload;
}

void menuMore(void)
{
  KEY_VALUES key_num;

  menuDrawPage(&moreItems);

  float curZcoordinate;
  
  while(infoMenu.menu[infoMenu.cur] == menuMore)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuHeat;
        break;

      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuFan;
        break;

      case KEY_ICON_2:
        #ifdef LOAD_UNLOAD_M701_M702
          if (isPrinting() && !isPause()) // need paused before extrude
          {
            setDialogText(LABEL_WARNING, LABEL_IS_PAUSE, LABEL_YES, LABEL_NO);
            showDialog(DIALOG_TYPE_ALERT, isPauseLoadUnload, NULL, NULL);
          }
          else
          {
            infoMenu.menu[++infoMenu.cur] = menuLoadUnload;
          }
        #else
          infoMenu.menu[++infoMenu.cur] = menuSendGcode;
        #endif
        break;
      
      case KEY_ICON_3:
        curZcoordinate = (infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS);
        if(curZcoordinate <= (FIRST_LAYER_HEIGHT + EPSILONE_FLOAT))
        {
          storeCmd("M418 P\n");
          infoMenu.menu[++infoMenu.cur] = menuZoffsetLiveSet;
        }       
        else
        {
          addToast(DIALOG_TYPE_ERROR, errorNotLay1);
        } 
        break;
      
      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuSendGcode;
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
