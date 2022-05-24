#include "PreheatMenu.h"
#include "includes.h"
#include "GPIO_Init.h"

#define TOGGLE_ON_PREHEAT   1 
#define TOGGLE_OFF_PREHEAT  2

#define PREHEAT_LIST_NUM_CM PREHEAT_COUNT-2

#define PREHEAT_BOTH_X_CM   40
#define PREHEAT_BOTH_Y_CM   200

#define PREHEAT_NOZZLE_X_CM 160
#define PREHEAT_NOZZLE_Y_CM 200

#define PREHEAT_BED_X_CM    280
#define PREHEAT_BED_Y_CM    200

const GUI_POINT preheat_title = {ICON_WIDTH/2, PREHEAT_TITLE_Y };
const GUI_POINT preheat_val_tool = {ICON_WIDTH - BYTE_WIDTH/2, PREHEAT_TOOL_Y};
const GUI_POINT preheat_val_bed = {ICON_WIDTH - BYTE_WIDTH/2, PREHEAT_BED_Y};

char startPreheatMsg[] = "예열을 시작합니다.";

/*
const ITEM itemToolPreheat[] = {
  // icon                        label
  {ICON_PREHEAT_BOTH,            LABEL_PREHEAT_BOTH},
  {ICON_BED,                     LABEL_BED},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_NOZZLE,                  LABEL_NOZZLE},
  {ICON_BACKGROUND,              LABEL_BACKGROUND},
  {ICON_BACKGROUND,              LABEL_BACKGROUND},
};
*/

// Redraw Preheat icon details
void refreshPreheatIcon(int8_t preheatnum, int8_t icon_index, const ITEM * menuitem)
{
  STRINGS_STORE preheatnames;
  W25Qxx_ReadBuffer((uint8_t*)&preheatnames,STRINGS_STORE_ADDR,sizeof(STRINGS_STORE));

  LIVE_INFO lvIcon;
  lvIcon.enabled[0] = true;
  lvIcon.enabled[1] = true;
  lvIcon.enabled[2] = true;

  //set preheat title properties
  lvIcon.lines[0].h_align = CENTER;
  lvIcon.lines[0].v_align = TOP;
  lvIcon.lines[0].fn_color = WHITE;
  lvIcon.lines[0].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[0].pos = preheat_title;
  lvIcon.lines[0].large_font = false;

  //set preheat tool propertites
  lvIcon.lines[1].h_align = RIGHT;
  lvIcon.lines[1].v_align = CENTER;
  lvIcon.lines[1].fn_color = WHITE;
  lvIcon.lines[1].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[1].pos = preheat_val_tool;
  lvIcon.lines[1].large_font = false;

  //set preheat bed properties
  lvIcon.lines[2].h_align = RIGHT;
  lvIcon.lines[2].v_align = CENTER;
  lvIcon.lines[2].fn_color = WHITE;
  lvIcon.lines[2].text_mode = GUI_TEXTMODE_TRANS;
  lvIcon.lines[2].pos = preheat_val_bed;
  lvIcon.lines[2].large_font = false;

  lvIcon.lines[0].text = (uint8_t *)preheatnames.preheat_name[preheatnum];

  char temptool[5];
  char tempbed[5];
  sprintf(temptool, "%d", infoSettings.preheat_temp[preheatnum]);
  sprintf(tempbed, "%d", infoSettings.preheat_bed[preheatnum]);
  lvIcon.lines[1].text = (uint8_t *)temptool;
  lvIcon.lines[2].text = (uint8_t *)tempbed;

  showLiveInfo(icon_index, &lvIcon, menuitem);
}

// draw toggle btn and label
void drawToggleBtn()
{
  ListItem_DisplayToggle(PREHEAT_BOTH_X_CM,   PREHEAT_BOTH_Y_CM,   ICONCHAR_TOGGLE_ON);  //Both
  ListItem_DisplayToggle(PREHEAT_NOZZLE_X_CM, PREHEAT_NOZZLE_Y_CM, ICONCHAR_TOGGLE_OFF); //Nozzle
  ListItem_DisplayToggle(PREHEAT_BED_X_CM,    PREHEAT_BED_Y_CM,    ICONCHAR_TOGGLE_OFF); //Bed

  char tempstr[50];
  sprintf(tempstr, "모두 예열");
  GUI_DispString(PREHEAT_BOTH_X_CM - 30,  PREHEAT_BOTH_Y_CM + 30,   (uint8_t *)tempstr);
  sprintf(tempstr, "노즐");
  GUI_DispString(PREHEAT_NOZZLE_X_CM + 2, PREHEAT_NOZZLE_Y_CM + 30, (uint8_t *)tempstr);
  sprintf(tempstr, "베드");
  GUI_DispString(PREHEAT_BED_X_CM + 2,    PREHEAT_BED_Y_CM + 30,    (uint8_t *)tempstr); //cremaker
}

void menuPreheat(void)
{
  uint16_t toggleNumBoth   = TOGGLE_ON_PREHEAT;
  uint16_t toggleNumNozzle = TOGGLE_OFF_PREHEAT;
  uint16_t toggleNumBed    = TOGGLE_OFF_PREHEAT;

  MENUITEMS preheatItems = {
    // title
    LABEL_PREHEAT,
    // icon                         label
    {{ICON_PREHEAT,                 LABEL_BACKGROUND},
     {ICON_PREHEAT,                 LABEL_BACKGROUND},
     {ICON_PREHEAT,                 LABEL_BACKGROUND},
     {ICON_PREHEAT,                 LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     //{ICON_PREHEAT_BOTH,            LABEL_PREHEAT_BOTH},
     {ICON_BACK,                    LABEL_BACK},}
  };  

  static TOOLPREHEAT nowHeater = BOTH;

  if(nowHeater!=BOTH)
  {
    nowHeater=BOTH;
  } //cremaker

  KEY_VALUES  key_num;

  //preheatItems.items[KEY_ICON_6] = itemToolPreheat[nowHeater];

  menuDrawPage(&preheatItems);
  for (int i = 0; i < PREHEAT_LIST_NUM_CM; i++) 
  {
    refreshPreheatIcon(i, i, &preheatItems.items[i]);
  } //cremaker
  
  drawToggleBtn();

  sendEncoder(1);

  while(infoMenu.menu[infoMenu.cur] == menuPreheat)
  {
    key_num = menuKeyGetValue();          

    switch(key_num)
    {
      case KEY_ICON_0:
      case KEY_ICON_1:
      case KEY_ICON_2:
      case KEY_ICON_3:
      //case KEY_ICON_4:
      //case KEY_ICON_5:
        switch(nowHeater)
        {
          case BOTH:
            heatSetTargetTemp(BED, infoSettings.preheat_bed[key_num]);
            heatSetTargetTemp(heatGetCurrentHotend(), infoSettings.preheat_temp[key_num]);
            break;
          case BED_PREHEAT:
            heatSetTargetTemp(BED, infoSettings.preheat_bed[key_num]);
            break;
          case NOZZLE0_PREHEAT:
            heatSetTargetTemp(heatGetCurrentHotend(), infoSettings.preheat_temp[key_num]);
            break;
        }
        refreshPreheatIcon(key_num, key_num, &preheatItems.items[key_num]);
        infoMenu.cur = infoMenu.cur - 3; //cremaker
        addToast(DIALOG_TYPE_INFO, startPreheatMsg);
        break;

      
      case KEY_ICON_4:
        if(toggleNumBoth==TOGGLE_ON_PREHEAT)
        {
          ListItem_DisplayToggle(PREHEAT_BOTH_X_CM,   PREHEAT_BOTH_Y_CM, ICONCHAR_TOGGLE_OFF);
          toggleNumBoth = TOGGLE_OFF_PREHEAT;
        }
        else if(toggleNumBoth==TOGGLE_OFF_PREHEAT)
        {
          ListItem_DisplayToggle(PREHEAT_BOTH_X_CM,   PREHEAT_BOTH_Y_CM,   ICONCHAR_TOGGLE_ON);
          ListItem_DisplayToggle(PREHEAT_NOZZLE_X_CM, PREHEAT_NOZZLE_Y_CM, ICONCHAR_TOGGLE_OFF);
          ListItem_DisplayToggle(PREHEAT_BED_X_CM,    PREHEAT_BED_Y_CM,    ICONCHAR_TOGGLE_OFF);
          toggleNumBoth   = TOGGLE_ON_PREHEAT;
          toggleNumNozzle = TOGGLE_OFF_PREHEAT;
          toggleNumBed    = TOGGLE_OFF_PREHEAT;
          nowHeater = BOTH;
        }      
        break;

      case KEY_ICON_5:
        if(toggleNumNozzle==TOGGLE_ON_PREHEAT)
        {
          ListItem_DisplayToggle(PREHEAT_NOZZLE_X_CM, PREHEAT_NOZZLE_Y_CM, ICONCHAR_TOGGLE_OFF);
          toggleNumNozzle=TOGGLE_OFF_PREHEAT;
        }
        else if(toggleNumNozzle==TOGGLE_OFF_PREHEAT)
        {
          ListItem_DisplayToggle(PREHEAT_BOTH_X_CM,   PREHEAT_BOTH_Y_CM,   ICONCHAR_TOGGLE_OFF);
          ListItem_DisplayToggle(PREHEAT_NOZZLE_X_CM, PREHEAT_NOZZLE_Y_CM, ICONCHAR_TOGGLE_ON);
          ListItem_DisplayToggle(PREHEAT_BED_X_CM,    PREHEAT_BED_Y_CM,    ICONCHAR_TOGGLE_OFF);
          toggleNumBoth   = TOGGLE_OFF_PREHEAT;
          toggleNumNozzle = TOGGLE_ON_PREHEAT;
          toggleNumBed    = TOGGLE_OFF_PREHEAT;
          nowHeater = NOZZLE0_PREHEAT;
        }
        break;

      case KEY_ICON_6:
        if(toggleNumBed==TOGGLE_ON_PREHEAT)
        {
          ListItem_DisplayToggle(PREHEAT_BED_X_CM,    PREHEAT_BED_X_CM,    ICONCHAR_TOGGLE_OFF);
          toggleNumBed=TOGGLE_OFF_PREHEAT;
        }
        else if(toggleNumBed==TOGGLE_OFF_PREHEAT)
        {
          ListItem_DisplayToggle(PREHEAT_BOTH_X_CM,   PREHEAT_BOTH_Y_CM,   ICONCHAR_TOGGLE_OFF);
          ListItem_DisplayToggle(PREHEAT_NOZZLE_X_CM, PREHEAT_NOZZLE_Y_CM, ICONCHAR_TOGGLE_OFF);
          ListItem_DisplayToggle(PREHEAT_BED_X_CM,    PREHEAT_BED_Y_CM,    ICONCHAR_TOGGLE_ON);
          toggleNumBoth   = TOGGLE_OFF_PREHEAT;
          toggleNumNozzle = TOGGLE_OFF_PREHEAT;
          toggleNumBed    = TOGGLE_ON_PREHEAT;
          nowHeater = BED_PREHEAT;
        }
        
        break;
        /*
        nowHeater = (TOOLPREHEAT)((nowHeater+1) % 3);
        preheatItems.items[key_num] = itemToolPreheat[nowHeater];
        menuDrawItem(&preheatItems.items[key_num], key_num);
        */
        
        
      case KEY_ICON_7:
        infoMenu.cur--; break;
      default:break;
    }
    loopProcess();
  }
}
