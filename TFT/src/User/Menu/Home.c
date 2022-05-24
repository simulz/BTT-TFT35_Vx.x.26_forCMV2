#include "Home.h"
#include "includes.h"

char tempMsg[]  = "원점으로 이동합니다.";
char tempMsgX[] = "X원점으로 이동합니다.";
char tempMsgY[] = "Y원점으로 이동합니다.";
char tempMsgZ[] = "Z원점으로 이동합니다.";

const MENUITEMS homeItems = {
  // title
  LABEL_HOME,
  // icon                         label
  {{ICON_HOME,                    LABEL_HOME},
   {ICON_X_HOME,                  LABEL_X},
   {ICON_Y_HOME,                  LABEL_Y},
   {ICON_Z_HOME,                  LABEL_Z},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACKGROUND,              LABEL_BACKGROUND},
   {ICON_BACK,                    LABEL_BACK},}
};

void menuHome(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&homeItems);
 
  GUI_DispString(40, 208, (uint8_t *)"정확한 측정을 위해");
  GUI_DispString(40, 236, (uint8_t *)"Home 전 온도를 올려주세요.");

  while(infoMenu.menu[infoMenu.cur] == menuHome)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0: storeCmd("G28\n");   addToast(DIALOG_TYPE_INFO, tempMsg);  break;
      case KEY_ICON_1: storeCmd("G28 X\n"); addToast(DIALOG_TYPE_INFO, tempMsgX); break;
      case KEY_ICON_2: storeCmd("G28 Y\n"); addToast(DIALOG_TYPE_INFO, tempMsgY); break;
      case KEY_ICON_3: storeCmd("G28 Z\n"); addToast(DIALOG_TYPE_INFO, tempMsgZ); break;
      case KEY_ICON_7: infoMenu.cur--;      break;
      default:break;
    }
    loopProcess();
  }
}

