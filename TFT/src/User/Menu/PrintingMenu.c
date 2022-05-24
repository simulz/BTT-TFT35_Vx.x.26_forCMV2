#include "Printing.h"
#include "includes.h"

REPRINT reprint;

const GUI_POINT printinfo_points[6] = {
    {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
    {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
    {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0},
    {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
    {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
    {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1},
};

const GUI_RECT printinfo_val_rect[6] = {
    {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

    {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

    {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_SM_EX, ICON_START_Y + PICON_HEIGHT * 0 + PICON_SPACE_Y * 0 + PICON_VAL_Y + BYTE_HEIGHT},

    {START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 0 + PICON_SPACE_X * 0 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},

    {START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 1 + PICON_SPACE_X * 1 + PICON_VAL_LG_EX, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},

    {START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_X, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y,
     START_X + PICON_LG_WIDTH * 2 + PICON_SPACE_X * 2 + PICON_VAL_SM_EX, ICON_START_Y + PICON_HEIGHT * 1 + PICON_SPACE_Y * 1 + PICON_VAL_Y + BYTE_HEIGHT},
};

const GUI_RECT rect_of_PS_right_range = {PS_ICONS_X, PS_ICONS_Y, LCD_WIDTH, LCD_HEIGHT};

const GUI_RECT rect_of_nozzle_value = {
  PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X,                  PS_ICONS_Y + PS_ICON_H*0 + PS_VALUE_GAP_Y*0,
  PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X + PS_STATE_VAL_W, PS_ICONS_Y + PS_ICON_H*0 + PS_VALUE_GAP_Y*0 + PS_STATE_VAL_H
};

const GUI_RECT rect_of_bed_value = {
  PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X,                  PS_ICONS_Y + PS_ICON_H*1 + PS_VALUE_GAP_Y*1,
  PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X + PS_STATE_VAL_W, PS_ICONS_Y + PS_ICON_H*1 + PS_VALUE_GAP_Y*1 + PS_STATE_VAL_H
};

const GUI_RECT rect_of_fan_value = {
  PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X,                  PS_ICONS_Y + PS_ICON_H*2 + PS_VALUE_GAP_Y*2,
  PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X + PS_STATE_VAL_W, PS_ICONS_Y + PS_ICON_H*2 + PS_VALUE_GAP_Y*2 + PS_STATE_VAL_H
};

const GUI_RECT rect_of_speed_value = {
  PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X,                  PS_ICONS_Y + PS_ICON_H*3 + PS_VALUE_GAP_Y*3,
  PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X + PS_STATE_VAL_W, PS_ICONS_Y + PS_ICON_H*3 + PS_VALUE_GAP_Y*3 + PS_STATE_VAL_H
};

const GUI_RECT rect_of_time_value = {
  PS_TIMER_X + PS_TIMER_W + PS_VALUE_GAP_X,                 PS_TIMER_Y + 10,
  PS_TIMER_X + PS_TIMER_W + PS_VALUE_GAP_X + PS_TIME_VAL_W, PS_TIMER_Y + 10 + PS_TIME_VAL_H
};

const GUI_RECT rect_of_layer_value = {
  PS_ICON_LAYER_X + PS_ICON_LAYER_W + PS_VALUE_GAP_X,                  PS_ICON_LAYER_Y,
  PS_ICON_LAYER_X + PS_ICON_LAYER_W + PS_VALUE_GAP_X + PS_STATE_VAL_W, PS_ICON_LAYER_Y + PS_STATE_VAL_H
};

static uint32_t nextLayerDrawTime = 0;
const  char *const Speed_ID[2] = {"속도", "압출량"}; //cremaker
char errorNoSource[100] = "저장장치를 찾을 수 없습니다.";
bool hasFilamentData;
uint16_t dxOfCompletNum = 0;
bool bedHeatTimeTest    = true;
bool nozzleHeatTimeTest = true;
bool lastPrinting;
uint8_t bedHeatTimeMin, bedHeatTimeSec, nozzleHeatTimeMin, nozzleHeatTimeSec;

#define TOGGLE_TIME 2000 // 1 seconds is 1000
#define LAYER_DRAW_TIME 1000 // 1 seconds is 1000

#define LAYER_TITLE "레이어" //cremaker
#define EXT_ICON_POS 0
#define BED_ICON_POS 1
#define FAN_ICON_POS 2
#define TIM_ICON_POS 3
#define Z_ICON_POS   4
#define SPD_ICON_POS 5

const ITEM itemIsPause[2] = {
  // icon        label
  {ICON_PAUSE,   LABEL_BACKGROUND},
  {ICON_PRINT,   LABEL_BACKGROUND},
};

const ITEM itemIsPrinting[4] = {
  // icon                        label
  {ICON_SUMMARY,     LABEL_SUMMARY},
  {ICON_REPEAT,      LABEL_REPRINT},
  {ICON_MAINMENU_PS, LABEL_MAIN_SCREEN},
  {ICON_BACK,        LABEL_BACK},
};

void menuBeforePrinting(void)
{
  //load stat/end/cancel gcodes from spi flash
  uint32_t size = 0;
  switch (infoFile.source)
  {
    case BOARD_SD: // GCode from file on ONBOARD SD
      size = request_M23_M36(infoFile.title + 5);
      //  if( powerFailedCreate(infoFile.title)==false)
      //  {
      //
      //  }    // FIXME: Powerfail resume is not yet supported for ONBOARD_SD. Need more work.

      if (size == 0)
      {
        ExitDir();
        infoMenu.cur--;
        return;
      }

      infoPrinting.size = size;

      //if(powerFailedExist())
      //{
      request_M24(0);
      //}
      //else
      //{
      //request_M24(infoBreakPoint.offset);
      //}

      if (infoMachineSettings.autoReportSDStatus == 1)
        request_M27(infoSettings.m27_refresh_time);  //Check if there is a SD or USB print running.
      else
        request_M27(0);

      infoHost.printing = true;  // Global lock info on printer is busy in printing.
      break;

    case TFT_UDISK:
    case TFT_SD:  // GCode from file on TFT SD
      if (f_open(&infoPrinting.file, infoFile.title, FA_OPEN_EXISTING | FA_READ) != FR_OK)
      {
        ExitDir();
        infoMenu.cur--;        
        return;
      }
      if (powerFailedCreate(infoFile.title) == false)
      {}
      powerFailedlSeek(&infoPrinting.file);

      infoPrinting.size = f_size(&infoPrinting.file);
      infoPrinting.cur = infoPrinting.file.fptr;
      if (infoSettings.send_start_gcode == 1 && infoPrinting.cur == 0)  // PLR continue printing, CAN NOT use start gcode
      {
        sendPrintCodes(0);
      }
      break;
    default:
      ExitDir();
      infoMenu.cur--;      
      return;
  }
  infoPrinting.printing = true;
  reprint.repeatPrint = false;
  infoPrinting.time = 0;
  infoPrinting.progress = 0;
  initPrintSummary();
  infoMenu.menu[infoMenu.cur] = menuPrinting;
}

void drawPrintingScreenCM(bool isPause)
{
  //GUI_ClearPrect(&ss_screen_without_toast);
  
  ICON_CustomReadDisplay(PS_MAINLOGO_X, PS_MAINLOGO_Y,  PRINTING_MAIN_LOGO_ADDR);
  ICON_ReadDisplay(PS_TIMER_X,          PS_TIMER_Y,             ICON_PRINTING_TIMER);

  if(infoPrinting.progress == 100) return;
  
  if(!isPause)
    ICON_ReadDisplay(PS_PAUSE_X,          PS_PAUSE_Y,             ICON_PAUSE);
  else
    ICON_ReadDisplay(PS_PAUSE_X,          PS_PAUSE_Y,             ICON_PRINT);

  ICON_ReadDisplay(PS_MORE_X,           PS_MORE_Y,              ICON_MORE);
  ICON_ReadDisplay(PS_STOP_PRINTING_X,  PS_STOP_PRINTING_Y,     ICON_STOP_PRINTING);
  //ICON_ReadDisplay(PS_STOP_PRINTING_X,  PS_STOP_PRINTING_Y,     ICON_PRINT);  
  ICON_ReadDisplay(PS_ICONS_X,          PS_ICONS_Y,             ICON_STATUS_ICONS);
  ICON_ReadDisplay(PS_ICON_LAYER_X,     PS_ICON_LAYER_Y,        ICON_STATUS_ICON_LAYER);
}

static inline void reValueNozzle(void)
{
  GUI_ClearPrect(&rect_of_nozzle_value);
  char tempstr[10];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(currentTool), heatGetTargetTemp(currentTool));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_DispString(PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X, PS_ICONS_Y + PS_ICON_H*0 + PS_VALUE_GAP_Y*0, (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reValueBed(void)
{
  GUI_ClearPrect(&rect_of_bed_value);
  char tempstr[10];
  sprintf(tempstr, "%d/%d", heatGetCurrentTemp(BED), heatGetTargetTemp(BED));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_DispString(PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X, PS_ICONS_Y + PS_ICON_H*1 + PS_VALUE_GAP_Y*1, (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawFan(void)
{
  GUI_ClearPrect(&rect_of_fan_value);
  char tempstr[10];
  if (infoSettings.fan_percentage == 1)
    sprintf(tempstr, "%d%%", fanGetCurPercent(currentFan));
  else
    sprintf(tempstr, "%d", fanGetCurSpeed(currentFan));

  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  GUI_DispString(PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X, PS_ICONS_Y + PS_ICON_H*2 + PS_VALUE_GAP_Y*2, (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawSpeed(void)
{
  GUI_ClearPrect(&rect_of_speed_value);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  char tempstr[10]; 
  sprintf(tempstr, "%d%%", speedGetCurPercent(currentSpeedID));
  GUI_DispString(PS_ICONS_X + PS_ICON_W + PS_VALUE_GAP_X, PS_ICONS_Y + PS_ICON_H*3 + PS_VALUE_GAP_Y*3, (uint8_t *)tempstr);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawTime()
{
  GUI_ClearPrect(&rect_of_time_value);
  uint8_t hour = infoPrinting.time / 3600,
     min = infoPrinting.time % 3600 / 60,
     sec = infoPrinting.time % 60;

  if (bedHeatTimeTest)
  {
    if (heatGetCurrentTemp(BED) > (heatGetTargetTemp(BED)-1))
    {
      bedHeatTimeMin = min;
      bedHeatTimeSec = sec;
      bedHeatTimeTest = false;
    }
  }

  if (nozzleHeatTimeTest && heatGetTargetTemp(heatGetCurrentHotend())!=0)
  {
    if (heatGetCurrentTemp(heatGetCurrentHotend()) > (heatGetTargetTemp(heatGetCurrentHotend())-3))
    {
      nozzleHeatTimeMin = min;
      nozzleHeatTimeSec = sec;
      nozzleHeatTimeTest = false;
    }
  }

  GUI_SetNumMode(GUI_NUMMODE_ZERO);
  GUI_SetTextMode(GUI_TEXTMODE_TRANS);
  char tempstr[10];
  sprintf(tempstr, "%02u:%02u:%02u", hour, min, sec);
  GUI_DispString(PS_TIMER_X + PS_TIMER_W + PS_VALUE_GAP_X, PS_TIMER_Y + 6, (uint8_t *)tempstr);
  GUI_SetNumMode(GUI_NUMMODE_SPACE);
  GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
}

static inline void reDrawProgress()
{
  if(infoPrinting.progress == 100)
  {
    ICON_CustomReadDisplay(PS_PROGRESS_NUM100_X + PS_PROGRESS_NUM_W*0 - 2, PS_PROGRESS_NUM100_Y,  PSCNUM_ICON_ADDR(1));
    ICON_CustomReadDisplay(PS_PROGRESS_NUM100_X + PS_PROGRESS_NUM_W*1 - 2, PS_PROGRESS_NUM100_Y,  PSCNUM_ICON_ADDR(0));
    ICON_CustomReadDisplay(PS_PROGRESS_NUM100_X + PS_PROGRESS_NUM_W*2 - 2, PS_PROGRESS_NUM100_Y,  PSCNUM_ICON_ADDR(0));
    return;
  }

  int percentValue = infoPrinting.progress / 10;
  dxOfCompletNum = PS_PROGRESS_NUM_W/2;

  switch(percentValue)
  {
    case 0:
      dxOfCompletNum = 0;
      break;
    case 1: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM10_X, PS_PROGRESS_NUM10_Y,  PSCNUM_ICON_ADDR(1));
      break;
    case 2: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM10_X, PS_PROGRESS_NUM10_Y,  PSCNUM_ICON_ADDR(2));
      break;
    case 3: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM10_X, PS_PROGRESS_NUM10_Y,  PSCNUM_ICON_ADDR(3));
      break;
    case 4: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM10_X, PS_PROGRESS_NUM10_Y,  PSCNUM_ICON_ADDR(4));
      break;
    case 5: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM10_X, PS_PROGRESS_NUM10_Y,  PSCNUM_ICON_ADDR(5)); 
      break;
    case 6: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM10_X, PS_PROGRESS_NUM10_Y,  PSCNUM_ICON_ADDR(6));      
      break;
    case 7: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM10_X, PS_PROGRESS_NUM10_Y,  PSCNUM_ICON_ADDR(7));
      break;
    case 8: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM10_X, PS_PROGRESS_NUM10_Y,  PSCNUM_ICON_ADDR(8));
      break;
    case 9: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM10_X, PS_PROGRESS_NUM10_Y,  PSCNUM_ICON_ADDR(9));
      break;
  }

  percentValue = infoPrinting.progress % 10;
  switch(percentValue)
  {
    case 0: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM1_X + dxOfCompletNum, PS_PROGRESS_NUM1_Y,  PSCNUM_ICON_ADDR(0));
      break;
    case 1: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM1_X + dxOfCompletNum, PS_PROGRESS_NUM1_Y,  PSCNUM_ICON_ADDR(1));
      break;
    case 2: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM1_X + dxOfCompletNum, PS_PROGRESS_NUM1_Y,  PSCNUM_ICON_ADDR(2));
      break;
    case 3: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM1_X + dxOfCompletNum, PS_PROGRESS_NUM1_Y,  PSCNUM_ICON_ADDR(3));
      break;
    case 4: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM1_X + dxOfCompletNum, PS_PROGRESS_NUM1_Y,  PSCNUM_ICON_ADDR(4));
      break;
    case 5: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM1_X + dxOfCompletNum, PS_PROGRESS_NUM1_Y,  PSCNUM_ICON_ADDR(5));
      break;
    case 6: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM1_X + dxOfCompletNum, PS_PROGRESS_NUM1_Y,  PSCNUM_ICON_ADDR(6));
      break;
    case 7: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM1_X + dxOfCompletNum, PS_PROGRESS_NUM1_Y,  PSCNUM_ICON_ADDR(7));
      break;
    case 8: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM1_X + dxOfCompletNum, PS_PROGRESS_NUM1_Y,  PSCNUM_ICON_ADDR(8));
      break;
    case 9: 
      ICON_CustomReadDisplay(PS_PROGRESS_NUM1_X + dxOfCompletNum, PS_PROGRESS_NUM1_Y,  PSCNUM_ICON_ADDR(9));
      break;
  }
}

static inline void reDrawLayer()
{  
  if (OS_GetTimeMs() > nextLayerDrawTime)
  {
    GUI_ClearPrect(&rect_of_layer_value);
    char tempstr[10];
    sprintf(tempstr, "%.2fmm",
            (infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS));

    GUI_SetTextMode(GUI_TEXTMODE_TRANS);
    GUI_DispString(PS_ICON_LAYER_X + PS_ICON_LAYER_W + PS_VALUE_GAP_X, PS_ICON_LAYER_Y, (uint8_t *)tempstr);
    GUI_SetTextMode(GUI_TEXTMODE_NORMAL);
    nextLayerDrawTime = OS_GetTimeMs() + LAYER_DRAW_TIME;
  }
}

static inline void toggleInfo(void)
{
  if (nextScreenUpdate(TOGGLE_TIME))
  {
    if (infoSettings.hotend_count > 1)
    {
      currentTool = (currentTool + 1) % infoSettings.hotend_count;
      RAPID_SERIAL_LOOP();  //perform backend printing loop before drawing to avoid printer idling
      reValueNozzle();
    }

    if ((infoSettings.fan_count + infoSettings.fan_ctrl_count) > 1)
    {
      currentFan = (currentFan + 1) % (infoSettings.fan_count + infoSettings.fan_ctrl_count);
      RAPID_SERIAL_LOOP();  //perform backend printing loop before drawing to avoid printer idling
      reDrawFan();
    }
    currentSpeedID = (currentSpeedID + 1) % 2;
    RAPID_SERIAL_LOOP();  //perform backend printing loop before drawing to avoid printer idling

    if(infoPrinting.progress != 100) reDrawSpeed();

    speedQuery();
    if (infoFile.source >= BOARD_SD)
      coordinateQuery(); 
    if (!hasFilamentData && isPrinting())
      updateFilamentUsed();
  }
}

static inline void printingDrawPage(void)
{
  reDrawProgress();
  reDrawTime();
  if(infoPrinting.progress == 100) return;
  reValueNozzle();
  reValueBed();
  reDrawFan();  
  //nextLayerDrawTime = 0; // Draw layer now
  reDrawLayer();
  reDrawSpeed();
}

void stopConfirm(void)
{  
  abortPrinting();
  infoMenu.cur--;
}

void printInfoPopup(void)
{
  uint8_t hour = infoPrintSummary.time / 3600;
  uint8_t min = infoPrintSummary.time % 3600 / 60;
  uint8_t sec = infoPrintSummary.time % 60;
  char showInfo[250];
  char tempstr[30];

  sprintf(showInfo, (char*)textSelect(LABEL_PRINT_TIME), hour, min, sec);

  if (infoPrintSummary.length == 0 && infoPrintSummary.weight == 0 && infoPrintSummary.cost == 0)
  {
    strcat(showInfo, (char *)textSelect(LABEL_NO_FILAMENT_STATS));
  }
  else
  {
    if (infoPrintSummary.length > 0)
    {
      sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_LENGTH), infoPrintSummary.length);
      strcat(showInfo, tempstr);
    }
    if (infoPrintSummary.weight > 0)
    {
      sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_WEIGHT), infoPrintSummary.weight);
      strcat(showInfo, tempstr);
    }
    // if (infoPrintSummary.cost > 0)
    // {
    //   sprintf(tempstr, (char *)textSelect(LABEL_FILAMENT_COST), infoPrintSummary.cost);
    //   strcat(showInfo, tempstr);
    // }
  }
  if(!bedHeatTimeTest && !nozzleHeatTimeTest)
  {
    sprintf(tempstr, "\n노즐 %02u:%02u, 베드 %02u:%02u", nozzleHeatTimeMin, nozzleHeatTimeSec, bedHeatTimeMin, bedHeatTimeSec);
    strcat(showInfo, tempstr);
  }
  
  popupReminder(DIALOG_TYPE_INFO, (uint8_t *)infoPrintSummary.name, (uint8_t *)showInfo);
}

void menuPrinting(void)
{
  char temp_info_printed[FILE_NUM + 50];

  //1title, ITEM_PER_PAGE item(icon + label)
  MENUITEMS printingItems = {
    //  title
    LABEL_BACKGROUND,
    // icon                       label
     {{ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND},
      {ICON_BACKGROUND, LABEL_BACKGROUND}}};

  uint8_t nowFan[MAX_FAN_COUNT] = {0};
  uint16_t curspeed[2] = {0};
  uint32_t time = 0;
  HEATER nowHeat;
  float curLayer = 0;
  bool lastPause = isPause(); //false
  lastPrinting = isPrinting();
  memset(&nowHeat, 0, sizeof(HEATER));

  if (lastPrinting == true)
  {
    if (infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD)
    {
      printingItems.title.address = (uint8_t *) infoFile.Longfile[infoFile.fileIndex];
    }
    else
    {
      printingItems.title.address = getCurGcodeName(infoFile.title);
      //printingItems.items[KEY_ICON_0] = itemIsPause[lastPause];
    }     
  }
  else // returned to this menu after a print was done (ex: after a popup)
  {
    printingItems.title.address = getCurGcodeName(infoFile.title);
    
    printingItems.items[KEY_ICON_2] = itemIsPrinting[0]; // Summary
    printingItems.items[KEY_ICON_3] = itemIsPrinting[1]; // Repeat print
    printingItems.items[KEY_ICON_6] = itemIsPrinting[2]; // Main screen
    printingItems.items[KEY_ICON_7] = itemIsPrinting[3]; // Back
  }  

  startTimeFW();
  menuDrawPage(&printingItems);
  drawPrintingScreenCM(lastPause);
  printingDrawPage();

  while (infoMenu.menu[infoMenu.cur] == menuPrinting)
  {
    // Scroll_DispString(&titleScroll, LEFT); //Scroll display file name will take too many CPU cycles
    if (infoPrinting.progress != 100)
    {
      //check nozzle temp change
      if (nowHeat.T[currentTool].current != heatGetCurrentTemp(currentTool) ||
          nowHeat.T[currentTool].target != heatGetTargetTemp(currentTool))
      {
        nowHeat.T[currentTool].current = heatGetCurrentTemp(currentTool);
        nowHeat.T[currentTool].target = heatGetTargetTemp(currentTool);
        RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
        reValueNozzle();
      }

      //check bed temp change
      if (nowHeat.T[BED].current != heatGetCurrentTemp(BED) || nowHeat.T[BED].target != heatGetTargetTemp(BED))
      {
        nowHeat.T[BED].current = heatGetCurrentTemp(BED);
        nowHeat.T[BED].target = heatGetTargetTemp(BED);
        RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
        reValueBed();
      }

      //check Fan speed change
      if (nowFan[currentFan] != fanGetCurSpeed(currentFan))
      {
        nowFan[currentFan] = fanGetCurSpeed(currentFan);
        RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
        reDrawFan();
      }      

      //Z_AXIS coordinate
      if (curLayer != ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS)))
      {
        curLayer = (infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(Z_AXIS) : coordinateGetAxisTarget(Z_AXIS);
        RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
        reDrawLayer();
      }

      //check change in speed or flow
      if (curspeed[currentSpeedID] != speedGetCurPercent(currentSpeedID))
      {
        curspeed[currentSpeedID] = speedGetCurPercent(currentSpeedID);
        RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
        reDrawSpeed();
      }
    }

    // check printing progress
    if (infoPrinting.size != 0)
    {
      //check print time change
      if (time != infoPrinting.time)
      {
        time = infoPrinting.time;
        RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
        reDrawTime();
      }

      //check progress value
      if(infoPrinting.progress != 100 && infoPrinting.progress != MIN((uint64_t)infoPrinting.cur * 100 / infoPrinting.size, 100))
      {        
        infoPrinting.progress = MIN((uint64_t)infoPrinting.cur * 100 / infoPrinting.size, 100);
        RAPID_SERIAL_LOOP(); //perform backend printing loop before drawing to avoid printer idling
        reDrawProgress();
      }
    }
    else
    {
      if (infoPrinting.progress != 100)
      {
        infoPrinting.progress = 100;
        reDrawTime();
        reDrawProgress();
      }
    }

    // check if print is paused
    if (lastPause != isPause())
    {
      lastPause = isPause();
      printingItems.items[KEY_ICON_0] = itemIsPause[lastPause];
      menuDrawItem(&printingItems.items[KEY_ICON_0], KEY_ICON_0);
    }

    // check if print just started or just finished
    if (lastPrinting != isPrinting())
    {
      lastPrinting = isPrinting();
      if (lastPrinting == true) // print is ongoing
      {
        return; // It will restart this interface if directly return this function without modify the value of infoMenu
      }
      else // print finished
      {
        GUI_ClearPrect(&rect_of_PS_right_range);

        printingItems.items[KEY_ICON_2] = itemIsPrinting[0]; // Summary
        printingItems.items[KEY_ICON_3] = itemIsPrinting[1]; // Repeat print
        printingItems.items[KEY_ICON_6] = itemIsPrinting[2]; // Main screen
        printingItems.items[KEY_ICON_7] = itemIsPrinting[3]; // Back
        menuDrawItem(&printingItems.items[KEY_ICON_2], KEY_ICON_2);
        menuDrawItem(&printingItems.items[KEY_ICON_3], KEY_ICON_3);
        menuDrawItem(&printingItems.items[KEY_ICON_6], KEY_ICON_6);
        menuDrawItem(&printingItems.items[KEY_ICON_7], KEY_ICON_7);
        preparePrintSummary();
        menuDrawPage(&printingItems);
        drawPrintingScreenCM(lastPause);
        printingDrawPage();
        storeCmd("M500\n");
      }
    }

    toggleInfo();

    KEY_VALUES key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        if (infoPrinting.progress != 100)
        {
          if (isPrinting())
          {
            storeCmd("M500\n");
            setPrintPause(!isPause(), false);
          }
        }
        break;

      case KEY_ICON_1:
        if(infoPrinting.progress != 100)
        {
          infoMenu.menu[++infoMenu.cur] = menuMore;
        }        
        break;

      case KEY_ICON_2:
        if (isPrinting())
        {
          setDialogText(LABEL_WARNING, LABEL_STOP_PRINT, LABEL_CONFIRM, LABEL_CANCEL);
          showDialog(DIALOG_TYPE_ALERT, stopConfirm, NULL, NULL);
          storeCmd("M500\n");
        }
        else
        {
          printInfoPopup();          
        }
        break;

      case KEY_ICON_3:   
        if(!(mountFS() == true))
        {
          addToast(DIALOG_TYPE_ERROR, errorNoSource);
          break;
        }             
        setInfoFileSource();
        reprint.repeatPrint = true; 

        sprintf(infoFile.title, reprint.lastPrintFilePath);
        sprintf(temp_info_printed, (char *)textSelect(LABEL_START_PRINT), reprint.lastPrintFilePath);
        setDialogText(LABEL_PRINT, (uint8_t *)temp_info_printed, LABEL_CONFIRM, LABEL_CANCEL);
        showDialog(DIALOG_TYPE_QUESTION, startPrint, ExitDir, NULL); 
        break;

      case KEY_ICON_6:
        exitPrinting();
        clearInfoFile();
        infoMenu.cur = 0;
        break;      

      case KEY_ICON_7:
        exitPrinting();
        infoMenu.cur--;
        break;

      default:
        break;
    }
    loopProcess();
  }
}
