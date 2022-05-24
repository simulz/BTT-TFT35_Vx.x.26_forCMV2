#include "StatusScreen.h"
#ifdef TFT70_V3_0
#define KEY_SPEEDMENU         KEY_ICON_3
#define KEY_FLOWMENU          (KEY_SPEEDMENU + 1)
#define KEY_MAINMENU          (KEY_FLOWMENU + 1)
#define SET_SPEEDMENUINDEX(x) setSpeedItemIndex(x)
#else
#define KEY_SPEEDMENU         KEY_ICON_3
#define KEY_MAINMENU          (KEY_SPEEDMENU + 1)
#define SET_SPEEDMENUINDEX(x)
#endif

const MENUITEMS StatusItems = {
  // title
  LABEL_BACKGROUND,
  // icon                         label
  {{ICON_BACKGROUND,             LABEL_BACKGROUND},
   {ICON_BACKGROUND,             LABEL_BACKGROUND},
   {ICON_BACKGROUND,             LABEL_BACKGROUND},
   {ICON_BACKGROUND,             LABEL_BACKGROUND},
   {ICON_BACKGROUND,             LABEL_BACKGROUND},
   {ICON_BACKGROUND,             LABEL_BACKGROUND},
   {ICON_BACKGROUND,             LABEL_BACKGROUND},
   {ICON_BACKGROUND,             LABEL_BACKGROUND},}
};

#define UPDATE_TOOL_TIME 2000 // 1 seconds is 1000
static int8_t lastConnection_status = -1;
static bool msgNeedRefresh = false;

static char msgtitle[20];
static char msgbody[MAX_MSG_LENGTH];
SCROLL msgScroll;

char connectMsg[] = "연결되었습니다."; //cremaker
bool firstBoot = true; //cremaker

const char *const SpeedID[2] = SPEED_ID;
// text position rectangles for Live icons
//icon 0
const GUI_POINT ss_title_point         = {SSICON_WIDTH - BYTE_WIDTH/2, SSICON_NAME_Y0};
const GUI_POINT ss_val_point           = {SSICON_WIDTH/2, SSICON_VAL_Y0};
const GUI_RECT ss_screen_without_toast = {0, TOAST_HEIGHT, LCD_WIDTH, LCD_HEIGHT};

#ifdef TFT70_V3_0
  const GUI_POINT ss_val2_point = {SSICON_WIDTH/2, SSICON_VAL2_Y0};
#endif

const  GUI_RECT msgRect ={SS_INFOBOX_X + INFOBOX_RADIUS_PIXEL,                   SS_INFOBOX_Y + INFOBOX_H/2 + 3, 
                          SS_INFOBOX_X + INFOBOX_W - INFOBOX_RADIUS_PIXEL, SS_INFOBOX_Y + INFOBOX_H - 2};

const GUI_RECT RecGantry = {START_X,                        1*SSICON_HEIGHT+0*SPACE_Y+ICON_START_Y + STATUS_GANTRY_YOFFSET,
                            4*ICON_WIDTH+3*SPACE_X+START_X, 1*ICON_HEIGHT+1*SPACE_Y+ICON_START_Y - STATUS_GANTRY_YOFFSET};

const GUI_RECT temperatureRect = {SS_ICONS_X + ICONS_W + ICONS_PIXEL_GAP,                  SS_ICONS_Y + ICONS_PIXEL_GAP*0 + ICONS_H*0 + 2 ,
                                  SS_ICONS_X + ICONS_W + ICONS_PIXEL_GAP + BYTE_WIDTH * 7, SS_ICONS_Y + ICONS_PIXEL_GAP*0 + ICONS_H*0 + 2 + BYTE_HEIGHT};

void drawStatusScreenCM(void)
{
  setMenuType(MENU_TYPE_ICON);
  
  TSC_ReDrawIcon = itemDrawIconPress;  

  GUI_ClearPrect(&ss_screen_without_toast);
  //GUI_Clear(BLACK);

  ICON_ReadDisplay(SS_ICONS_X,          SS_ICONS_Y,     ICON_STATUS_ICONS);
  ICON_CustomReadDisplay(SS_MAINLOGO_X, SS_MAINLOGO_Y,  STATUS_CM_LOGO_ADDR);
  ICON_ReadDisplay(SS_XYZICON_X,        SS_XYZICON_Y,   ICON_STATUS_XYZ);
  ICON_ReadDisplay(SS_MAINMENU_X,       SS_MAINMENU_Y,  ICON_MAINMENU);
  ICON_CustomReadDisplay(SS_INFOBOX_X,  SS_INFOBOX_Y,   INFOBOX_ADDR);
  ICON_ReadDisplay(SS_PRINT_X,          SS_PRINT_Y,     ICON_PRINT);
}

void drawValuesSSCM(void)
{
  char tempstr[45];

  GUI_ClearPrect(&temperatureRect);
  // draw nozzle temperature
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));
  GUI_DispString(SS_ICONS_X + ICONS_W + ICONS_PIXEL_GAP, SS_ICONS_Y + ICONS_PIXEL_GAP*0 + ICONS_H*0 + 2 ,(uint8_t *)tempstr);

  // draw bed temperature
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));  
  GUI_DispString(SS_ICONS_X + ICONS_W + ICONS_PIXEL_GAP, SS_ICONS_Y + ICONS_PIXEL_GAP*1 + ICONS_H*1 + 2 ,(uint8_t *)tempstr);

  // draw fan speed
  sprintf(tempstr, "%d%%", fanGetCurPercent(currentFan));
  GUI_DispString(SS_ICONS_X + ICONS_W + ICONS_PIXEL_GAP, SS_ICONS_Y + ICONS_PIXEL_GAP*2 + ICONS_H*2 + 2 ,(uint8_t *)tempstr);

  // draw speed
  sprintf(tempstr, "%d%%", speedGetCurPercent(0));
  GUI_DispString(SS_ICONS_X + ICONS_W + ICONS_PIXEL_GAP, SS_ICONS_Y + ICONS_PIXEL_GAP*3 + ICONS_H*3 + 2 ,(uint8_t *)tempstr);


  // draw x value
  sprintf(tempstr, "%.2f", coordinateGetAxisActual(X_AXIS));
  GUI_DispStringRight(SS_XYZVALUE_X, SS_XYZVALUE_Y + XYZ_ICON_PIXEL_GAP*0 + XYZ_ICON_H*0 + 2 ,(uint8_t *)tempstr);

  // draw y value
  sprintf(tempstr, "%.2f", coordinateGetAxisActual(Y_AXIS));
  GUI_DispStringRight(SS_XYZVALUE_X, SS_XYZVALUE_Y + XYZ_ICON_PIXEL_GAP*1 + XYZ_ICON_H*1 + 2 ,(uint8_t *)tempstr);

  // draw z value
  sprintf(tempstr, "%.2f", coordinateGetAxisActual(Z_AXIS));
  GUI_DispStringRight(SS_XYZVALUE_X, SS_XYZVALUE_Y + XYZ_ICON_PIXEL_GAP*2 + XYZ_ICON_H*2 + 2 ,(uint8_t *)tempstr);
}

void statusScreen_setMsg(const uint8_t *title, const uint8_t *msg)
{
  strncpy(msgtitle, (char *)title, sizeof(msgtitle));
  strncpy(msgbody, (char *)msg, sizeof(msgbody));
  msgNeedRefresh = true;
}

void statusScreen_setReady(void)
{
  strncpy(msgtitle, (char *)textSelect(LABEL_STATUS), sizeof(msgtitle));
  if (infoHost.connected == false)
  {
    strncpy(msgbody, (char *)textSelect(LABEL_UNCONNECTED), sizeof(msgbody));
  }
  else
  {
    strncpy(msgbody, (char *)machine_type, sizeof(msgbody));
    strcat(msgbody, " ");
    strcat(msgbody, (char *)textSelect(LABEL_READY));
    if(firstBoot)
    {
      addToast(DIALOG_TYPE_INFO, connectMsg); //cremaker
      firstBoot = false;
    }
    
  }
  msgNeedRefresh = true;
}

void drawStatusScreenMsg(void)
{
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);

  ICON_CustomReadDisplay(rect_of_keySSCM[1].x0, rect_of_keySSCM[1].y0, INFOBOX_ADDR);
  GUI_SetColor(INFOMSG_BKCOLOR);
  GUI_DispString(rect_of_keySSCM[1].x0 + STATUS_MSG_ICON_XOFFSET,
                 rect_of_keySSCM[1].y0 + STATUS_MSG_ICON_YOFFSET,
                 IconCharSelect(ICONCHAR_INFO));

  GUI_DispString(rect_of_keySSCM[1].x0 + BYTE_HEIGHT + STATUS_MSG_TITLE_XOFFSET,
                 rect_of_keySSCM[1].y0 + STATUS_MSG_ICON_YOFFSET,
                 (uint8_t *)msgtitle);
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_FillPrect(&msgRect);

  Scroll_CreatePara(&msgScroll, (uint8_t *)msgbody, &msgRect);

  GUI_RestoreColorDefault();

  msgNeedRefresh = false;
}

static inline void scrollMsg(void)
{
  GUI_SetBkColor(INFOMSG_BKCOLOR);
  GUI_SetColor(INFOMSG_COLOR);
  Scroll_DispString(&msgScroll,CENTER);
  GUI_RestoreColorDefault();
}

static inline void toggleTool(void)
{
  if (nextScreenUpdate(UPDATE_TOOL_TIME))
  {
    if (infoSettings.hotend_count > 1)
    {
      currentTool = (currentTool+1) % infoSettings.hotend_count;
    }
    if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
    {
      currentFan = (currentFan + 1) % (infoSettings.fan_count + infoSettings.fan_ctrl_count);
    }
    currentSpeedID = (currentSpeedID + 1) % 2;
    //drawTemperature();
    drawValuesSSCM();

    // gcode queries must be call after drawTemperature
    coordinateQuery();
    speedQuery();
    fanSpeedQuery();
  }
}

void sendCMDsInStatusScreen(void)
{
  storeCmd("M408\n");
  storeCmd("M2105 P\n");

  if (infoSettings.z_homing_protect)
    storeCmd("M411 T\n");
  else if (!infoSettings.z_homing_protect)
    storeCmd("M411 F\n");
}

void menuStatus(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  char testDebugNum[10];

  GUI_SetBkColor(infoSettings.bg_color);    
  menuDrawPage(&StatusItems);
  drawStatusScreenCM();
  GUI_SetColor(WHITE);
  drawValuesSSCM();
  drawStatusScreenMsg();
  sendCMDsInStatusScreen();  

  while (infoMenu.menu[infoMenu.cur] == menuStatus)
  {
    if(infoHost.connected != lastConnection_status)
    {
      statusScreen_setReady();
      lastConnection_status = infoHost.connected;
    }
    if (msgNeedRefresh)
    {
      drawStatusScreenMsg();
    }
    scrollMsg();
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuMain;
        break;
      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuPrint;
        break;
      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuNotification;
      default:
        break;
    }
    toggleTool();
    loopProcess();
  }
}
