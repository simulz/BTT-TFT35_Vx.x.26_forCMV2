#include "includes.h"
#include "Printing.h"

PRINTING infoPrinting;
PRINTSUMMARY infoPrintSummary = {.name[0] = '\0', 0, 0, 0, 0, 0};

bool filamentRunoutAlarm;

uint8_t gcode_count_len = 0;

static bool updateM27_waiting = false;
static float last_E_pos;
static float nozzle_area = 0.02404; 
static float E_pos_per_time = 0;
static float fila_weight_per_time = 0;
static uint32_t beginTimeFW = 0;
static uint32_t curTimeFW = 0;


bool isHostPrinting(void)
{
  return (infoHost.printing);
}

void startTimeFW(void)
{
  beginTimeFW = OS_GetTimeMs();  
}

bool isPrinting(void)
{
  return infoPrinting.printing;
}

bool isPause(void)
{
  return infoPrinting.pause;
}

bool isM0_Pause(void)
{
  return infoPrinting.m0_pause;
}

void setPrintingTime(uint32_t RTtime)
{
  if (RTtime%1000 == 0)
  {
    if (isPrinting() && !isPause())
    {
      infoPrinting.time++;
    }
  }
}

uint32_t getPrintSize(void)
{
  return infoPrinting.size;
}

void setPrintSize(uint32_t size)
{
  infoPrinting.size = size;
}

uint32_t getPrintCur(void)
{
  return infoPrinting.cur;
}

void setPrintCur(uint32_t cur)
{
  infoPrinting.cur = cur;
}

bool getPrintRunout(void)
{
  return infoPrinting.runout;
}

void setPrintRunout(bool runout)
{
  infoPrinting.runout = runout;
}

void setRunoutAlarmTrue(void)
{
  filamentRunoutAlarm = true;
}

void setRunoutAlarmFalse(void)
{
  filamentRunoutAlarm = false;
}

bool getRunoutAlarm(void)
{
  return filamentRunoutAlarm;
}

void setPrintModelIcon(bool exist)
{
  infoPrinting.model_icon = exist;
}

bool getPrintModelIcon(void)
{
  return infoPrinting.model_icon;
}

uint8_t getPrintProgress(void)
{
  return infoPrinting.progress;
}

uint32_t getPrintTime(void)
{
  return infoPrinting.time;
}

void printSetUpdateWaiting(bool isWaiting)
{
  updateM27_waiting = isWaiting;
}

//only return gcode file name except path
//for example:"SD:/test/123.gcode"
//only return "123.gcode"
uint8_t *getCurGcodeName(char *path)
{
  char * name = strrchr(path, '/');
  if (name != NULL)
    return (uint8_t *)(name + 1);
  else
    return (uint8_t *)path;
}

//send print codes [0: start gcode, 1: end gcode 2: cancel gcode 3: end autoprint Gcode]
void sendPrintCodes(uint8_t index)
{
  PRINT_GCODES printcodes;
  W25Qxx_ReadBuffer((uint8_t*)&printcodes,PRINT_GCODES_ADDR,sizeof(PRINT_GCODES));
  switch (index)
  {
  case 0:
    mustStoreScript(printcodes.start_gcode);
    break;
  case 1:
    mustStoreScript(printcodes.end_gcode);
    break;
  case 2:
    mustStoreScript(printcodes.cancel_gcode);
    break;
  case 3:
    mustStoreScript(printcodes.end_autoprint_gcode);
    break;

  default:
    break;
  }
}

void initPrintSummary(void)
{
  last_E_pos = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));
  infoPrintSummary = (PRINTSUMMARY){.name[0] = '\0', 0, 0, 0, 0};
  hasFilamentData = false;
}

void preparePrintSummary(void)
{
  if (infoMachineSettings.long_filename_support == ENABLED && infoFile.source == BOARD_SD)
   sprintf(infoPrintSummary.name,"%." STRINGIFY(SUMMARY_NAME_LEN) "s", infoFile.Longfile[infoFile.fileIndex]);
  else
   sprintf(infoPrintSummary.name,"%." STRINGIFY(SUMMARY_NAME_LEN) "s", getCurGcodeName(infoFile.title));

  infoPrintSummary.time = infoPrinting.time;

  if (speedGetCurPercent(1) != 100)
  {
    infoPrintSummary.length = (infoPrintSummary.length * speedGetCurPercent(1)) / 100;  // multiply by flow percentage
    infoPrintSummary.weight = (infoPrintSummary.weight * speedGetCurPercent(1)) / 100;  // multiply by flow percentage
    infoPrintSummary.cost   = (infoPrintSummary.cost   * speedGetCurPercent(1)) / 100;  // multiply by flow percentage
  }
}

void updateFilamentUsed(void)
{  
  char tempstr[30];

  float E_pos = ((infoFile.source >= BOARD_SD) ? coordinateGetAxisActual(E_AXIS) : coordinateGetAxisTarget(E_AXIS));
  if ((E_pos + MAX_RETRACT_LIMIT) < last_E_pos) //Check whether E position reset (G92 E0)
  {
    last_E_pos = 0;
  }
  infoPrintSummary.length += (E_pos - last_E_pos) / 1000;
  E_pos_per_time += (E_pos - last_E_pos) / 10; // unit is 'cm'
  last_E_pos = E_pos;

  curTimeFW = OS_GetTimeMs();

  if(curTimeFW-beginTimeFW > 5000)
  {
    beginTimeFW = OS_GetTimeMs();

    fila_weight_per_time = filaweight.filaDensity * nozzle_area * E_pos_per_time;
    sprintf(tempstr,"M408 U%.4f\n",fila_weight_per_time);
    storeCmd(tempstr);

    infoPrintSummary.weight += fila_weight_per_time;

    E_pos_per_time = 0;
    fila_weight_per_time = 0;    
  }
}

static inline void setM0Pause(bool m0_pause)
{
  infoPrinting.m0_pause = m0_pause;
}

bool setPrintPause(bool is_pause, bool is_m0pause)
{
  static bool pauseLock = false;
  if (pauseLock)                      return false;
  if (!isPrinting())                  return false;
  if (infoPrinting.pause == is_pause) return false;

  pauseLock = true;
  switch (infoFile.source)
  {
    case BOARD_SD:
    case BOARD_SD_REMOTE:
      infoPrinting.pause = is_pause;
      if (is_pause)
        request_M25();
      else
        request_M24(0);
      break;

    case TFT_UDISK:
    case TFT_SD:
      infoPrinting.pause = is_pause;
      if (infoPrinting.pause == true && is_m0pause == false)      
        loopProcessToCondition(&isNotEmptyCmdQueue);

      bool isCoorRelative = coorGetRelative();
      bool isExtrudeRelative = eGetRelative();
      static COORDINATE tmp;

      if (infoPrinting.pause)
      {
        //restore status before pause
        //if pause was triggered through M0/M1 then break
        if (is_m0pause == true)
        {
          setM0Pause(is_m0pause);
          popupReminder(DIALOG_TYPE_ALERT, LABEL_PAUSE, LABEL_PAUSE);
          break;
        }

        coordinateGetAll(&tmp);

        if (isCoorRelative == true)     mustStoreCmd("G90\n");
        if (isExtrudeRelative == true)  mustStoreCmd("M82\n");

        if (heatGetCurrentTemp(heatGetCurrentHotend()) > infoSettings.min_ext_temp)
        {
          mustStoreCmd("G1 E%.5f F%d\n", tmp.axis[E_AXIS] - infoSettings.pause_retract_len,
                       infoSettings.pause_feedrate[FEEDRATE_E]);
        }
        if (coordinateIsKnown())
        {
          mustStoreCmd("G1 Z%.3f F%d\n", tmp.axis[Z_AXIS] + infoSettings.pause_z_raise, infoSettings.pause_feedrate[FEEDRATE_Z]);
          mustStoreCmd("G1 X%.3f Y%.3f F%d\n", infoSettings.pause_pos[X_AXIS], infoSettings.pause_pos[Y_AXIS],
                       infoSettings.pause_feedrate[FEEDRATE_XY]);
        }

        if (isCoorRelative == true)     mustStoreCmd("G91\n");
        if (isExtrudeRelative == true)  mustStoreCmd("M83\n");
      }
      else
      {
        if (isM0_Pause() == true)
        {
          setM0Pause(is_m0pause);
          breakAndContinue();
          break;
        }
        if (isCoorRelative == true)     mustStoreCmd("G90\n");
        if (isExtrudeRelative == true)  mustStoreCmd("M82\n");

        if (coordinateIsKnown())
        {
          mustStoreCmd("G1 X%.3f Y%.3f F%d\n", tmp.axis[X_AXIS], tmp.axis[Y_AXIS], infoSettings.pause_feedrate[FEEDRATE_XY]);
          mustStoreCmd("G1 Z%.3f F%d\n", tmp.axis[Z_AXIS], infoSettings.pause_feedrate[FEEDRATE_Z]);
        }
        if (heatGetCurrentTemp(heatGetCurrentHotend()) > infoSettings.min_ext_temp)
        {
          mustStoreCmd("G1 E%.5f F%d\n", tmp.axis[E_AXIS] - infoSettings.pause_retract_len + infoSettings.resume_purge_len,
                       infoSettings.pause_feedrate[FEEDRATE_E]);
        }
        mustStoreCmd("G92 E%.5f\n", tmp.axis[E_AXIS]);
        mustStoreCmd("G1 F%d\n", tmp.feedrate);

        if (isCoorRelative == true)     mustStoreCmd("G91\n");
        if (isExtrudeRelative == true)  mustStoreCmd("M83\n");
      }
      break;
  }
  pauseLock = false;
  return true;
}

void exitPrinting(void)
{
  memset(&infoPrinting, 0, sizeof(PRINTING));
  ExitDir();
}

void endPrinting(void)
{
  if (!infoPrinting.printing) return;

  switch (infoFile.source)
  {
    case BOARD_SD:
    case BOARD_SD_REMOTE:
      break;

    case TFT_UDISK:
    case TFT_SD:
      f_close(&infoPrinting.file);
      break;
  }
  
  powerFailedClose();
  powerFailedDelete();

  infoPrinting.cur = infoPrinting.size;  // always update the print progress to 100% even if the print terminated
  infoPrinting.printing = infoPrinting.pause = false;

  preparePrintSummary();  // update print summary. infoPrinting are used


  if (infoSettings.send_end_gcode == 1)
  {
    if(isAutoPrinting)
    {
      sendPrintCodes(3);
    }
    else
    {
      sendPrintCodes(1);
    }
    
    if(!rightOrLeft && isAutoPrinting)
    {
      Serial_Puts(SERIAL_PORT_3,"CBSR\n");
    }
    else if(rightOrLeft && isAutoPrinting)
    {
      Serial_Puts(SERIAL_PORT_3,"CBSL\n");
    }    
  }
}

void printingFinished(void)
{
  BUZZER_PLAY(sound_success);
  endPrinting();
  if (infoSettings.auto_off) // Auto shut down after printing
  {
    startShutdown();
  }
}

void abortPrinting(void)
{
  // used to avoid a possible loop in case an abort gcode (e.g. M524) is present in
  // the queue infoCmd and the function loopProcess() is invoked by this function
  static bool loopDetected = false;

  if (loopDetected) return;
  if (!infoPrinting.printing) return;  

  switch (infoFile.source)
  {
    case BOARD_SD:
    case BOARD_SD_REMOTE:
      // infoHost.printing = false;  // Not so fast! Let Marlin tell that he's done!
      //Several M108 are sent to Marlin because consecutive blocking operations
      // such as heating bed, extruder may defer processing of M524
      breakAndContinue();
      breakAndContinue();
      breakAndContinue();
      breakAndContinue();
      if (infoMachineSettings.firmwareType == FW_REPRAPFW)
        request_M0(); // M524 is not supportet in reprap firmware
      else
        request_M524();

      if (infoHost.printing)
      {
        setDialogText(LABEL_SCREEN_INFO, LABEL_BUSY, LABEL_BACKGROUND, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);

        loopProcessToCondition(&isHostPrinting);  // wait for the printer to settle down
      }

      // setDialogText(LABEL_SCREEN_INFO, LABEL_BUSY, LABEL_BACKGROUND, LABEL_BACKGROUND);
      // showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
      // while (infoHost.printing == true)  // wait for the printer to settle down
      // {
      //   loopProcess();
      // }
      // infoMenu.cur--;
      break;

    case TFT_UDISK:
    case TFT_SD:
      clearCmdQueue();
      break;
  }  

  if (infoSettings.send_cancel_gcode == 1)
  //{
    sendPrintCodes(2);
  //   addToast(DIALOG_TYPE_INFO,"cancel G");
  // }
  // else
  // {
  //   addToast(DIALOG_TYPE_INFO,"CG != 1");
  // }  

  heatClearIsWaiting();
  endPrinting();  
  exitPrinting();

  

  loopDetected = false;
}

// Shut down menu, when the hotend temperature is higher than "AUTO_SHUT_DOWN_MAXTEMP"
// wait for cool down, in the meantime, you can shut down by force
void shutdown(void)
{
  for(uint8_t i = 0; i < infoSettings.fan_count; i++)
  {
    if (fanIsType(i, FAN_TYPE_F)) mustStoreCmd("%s S0\n", fanCmd[i]);
  }
  mustStoreCmd("M81\n");
  popupReminder(DIALOG_TYPE_INFO, LABEL_SHUT_DOWN, LABEL_SHUTTING_DOWN);
}

void shutdownLoop(void)
{
  bool tempIsLower = true;
  for (uint8_t i = NOZZLE0; i < infoSettings.hotend_count; i++)
  {
    if (heatGetCurrentTemp(i) >= AUTO_SHUT_DOWN_MAXTEMP)
      tempIsLower = false;
  }
  if (tempIsLower)
  {
   shutdown();
  }
}

void startShutdown(void)
{
  char tempstr[75];
  LABELCHAR(tempbody, LABEL_WAIT_TEMP_SHUT_DOWN);
  sprintf(tempstr, tempbody, infoSettings.auto_off_temp);

  for(uint8_t i = 0; i < infoSettings.fan_count; i++)
  {
    if (fanIsType(i,FAN_TYPE_F)) mustStoreCmd("%s S255\n", fanCmd[i]);
  }
  setDialogText(LABEL_SHUT_DOWN, (uint8_t *)tempstr, LABEL_FORCE_SHUT_DOWN, LABEL_CANCEL);
  showDialog(DIALOG_TYPE_INFO, shutdown, NULL, shutdownLoop);
}

// get gcode command from sd card
void getGcodeFromFile(void)
{
  if (!infoPrinting.printing) return;
  if (infoFile.source >= BOARD_SD)  return;
  if (heatHasWaiting() || isNotEmptyCmdQueue() || infoPrinting.pause) return;
  if (moveCacheToCmd() == true) return;  
  
  powerFailedCache(infoPrinting.file.fptr); // update Power-loss Recovery file

  CMD      gcode = {};
  uint8_t  gcode_count = 0;
  bool     sd_comment_mode = false;
  bool     sd_comment_space = true;
  char     read_char = '0'; //read_char //sd_char
  UINT     br = 0;
  FIL *    ip_file = &infoPrinting.file;
  uint32_t ip_cur  = infoPrinting.cur;
  uint32_t ip_size = infoPrinting.size;

  infoPrinting.noise_filter_cur = infoPrinting.cur;

  for(;ip_cur < ip_size;)
  {
    if (f_read(ip_file, &read_char, 1, &br) != FR_OK)
    {
      ip_cur = ip_size; 
      continue; // "continue" will force also to execute "ip_cur++" in the "for" statement
    }

    ip_cur++;

    if (read_char == '\n')  //'\n' is end flag for per command
    {
      if(gcode_count != 0)
      {        
        gcode[gcode_count++] = '\n';
        gcode[gcode_count]   = 0;      //terminate string
        gcode_count_len = gcode_count; //save value to check gcode length, start num : 0
        storeCmdFromUART(SERIAL_PORT, gcode, TFT_DISK);
        if(gcode_count >0)
          break;
      }

      sd_comment_mode  = false;  // for new command
      sd_comment_space = true;
    }
    else if (gcode_count >= CMD_MAX_SIZE - 2)
    {
    }  //when the command length beyond the maximum, ignore the following bytes
    else
    {
      if (read_char == ';')  //';' is comment out flag
      {
        sd_comment_mode = true;
        sd_comment_space = true;
      }
      else
      {
        if (sd_comment_space && (read_char== 'G'||read_char == 'M'||read_char == 'T'))  //ignore ' ' space bytes
        ///if (sd_comment_space && read_char != ' ')
          sd_comment_space = false;
        if (!sd_comment_mode && !sd_comment_space && read_char != '\r')  //normal gcode
          gcode[gcode_count++] = read_char;
      }
    }    
  }

  infoPrinting.cur = ip_cur; // update infoPrinting.cur with current file position

  // end of .gcode file
  if(ip_cur == ip_size) // in case of end of gcode file, finalize the print
  {
    printingFinished();    
  }
  else if(ip_cur > ip_size) // in case of print abort (ip_cur == ip_size + 1), display an error message and abort the print
  {
    BUZZER_PLAY(sound_error);
    popupReminder(DIALOG_TYPE_ERROR, (infoFile.source == TFT_SD) ? LABEL_READ_TFTSD_ERROR : LABEL_READ_U_DISK_ERROR, LABEL_PROCESS_ABORTED);

    abortPrinting();
  }
}

void breakAndContinue(void)
{
  setRunoutAlarmFalse();
  clearCmdQueue();
  Serial_Puts(SERIAL_PORT, "M108\n");
}

void resumeAndPurge(void)
{
  setRunoutAlarmFalse();
  clearCmdQueue();
  Serial_Puts(SERIAL_PORT, "M876 S0\n");
}

void resumeAndContinue(void)
{
  setRunoutAlarmFalse();
  clearCmdQueue();
  Serial_Puts(SERIAL_PORT, "M876 S1\n");
}

bool hasPrintingMenu(void)
{
  for (uint8_t i = 0; i <= infoMenu.cur; i++)
  {
    if (infoMenu.menu[i] == menuPrinting) return true;
  }
  return false;
}

void loopCheckPrinting(void)
{
  #ifdef HAS_EMULATOR
    if (infoMenu.menu[infoMenu.cur] == menuMarlinMode) return;
  #endif

  if (infoHost.printing && !infoPrinting.printing)
  {
    infoPrinting.printing = true;
    if (!hasPrintingMenu())
    {
      infoMenu.menu[++infoMenu.cur] = menuPrinting;
    }
  }

  if (infoFile.source < BOARD_SD) return;
  if (infoMachineSettings.autoReportSDStatus == ENABLED) return;
  if (!infoSettings.m27_active && !infoPrinting.printing) return;

  static uint32_t nextCheckPrintTime = 0;
  uint32_t update_M27_time = infoSettings.m27_refresh_time * 1000;

  do
  {  /* WAIT FOR M27  */
    if (updateM27_waiting == true)
    {
      nextCheckPrintTime = OS_GetTimeMs() + update_M27_time;
      break;
    }
    if (OS_GetTimeMs() < nextCheckPrintTime)
      break;
    if (storeCmd("M27\n") == false)
      break;
    nextCheckPrintTime = OS_GetTimeMs() + update_M27_time;
    updateM27_waiting = true;
  } while(0);
}
