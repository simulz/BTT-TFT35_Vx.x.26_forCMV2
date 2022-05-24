#include "includes.h"
#include "parseACK.h"

LOADCHECKFILAMENT loadCheckFilament;
TESTING infoTesting;

char dmaL2Cache[ACK_MAX_SIZE];
char echoKr[] = "알림";
char errorKr[] = "에러";
static uint16_t ack_index=0;
static uint8_t ack_cur_src = SERIAL_PORT;
uint16_t dmaL2Cache_len;

uint8_t portNum[SERIAL_PORT_COUNT] = {SERIAL_PORT, SERIAL_PORT_2, SERIAL_PORT_3, SERIAL_PORT_4};
bool portSeen[_UART_CNT] = {false, false, false, false, false, false};

struct HOST_ACTION
{
  char prompt_begin[20];
  char prompt_button1[20];
  char prompt_button2[20];
  bool prompt_show;         //Show popup reminder or not
  uint8_t button;           //Number of buttons
} hostAction;

// notify or ignore messages starting with following text
const ECHO knownEcho[] = {
//  {ECHO_NOTIFY_NONE, "enqueueing \"M117\""},
  {ECHO_NOTIFY_NONE,  "busy: paused for user",   "작동중: 정지됨"},
  {ECHO_NOTIFY_NONE,  "busy: processing",        "작동중: 진행중"},
  {ECHO_NOTIFY_NONE,  "Now fresh file:",         "새로고침 파일:"},
  {ECHO_NOTIFY_NONE,  "Now doing file:",         "작동 파일:"},
  {ECHO_NOTIFY_NONE,  "Probe Offset",            "프로브 오프셋"},
  {ECHO_NOTIFY_NONE,  "Flow:",                   "압출"},
  {ECHO_NOTIFY_NONE,  "echo:;",                  "알림:;"},                //M503
  {ECHO_NOTIFY_NONE,  "echo:  G",                "알림:  G"},              //M503
  {ECHO_NOTIFY_NONE,  "echo:  M",                "알림:  M"},              //M503
  {ECHO_NOTIFY_NONE,  "Cap:",                    "Cap:"},                 //M115
  {ECHO_NOTIFY_NONE,  "Config:",                 "Config:"},              //M360
  {ECHO_NOTIFY_TOAST, "설정 저장됨",              "설정 저장됨"},           //M500
  {ECHO_NOTIFY_TOAST, "echo:Bed",                "알림:베드"},             //M420
  {ECHO_NOTIFY_NONE,  "echo:Fade",               "알림:Fade"},            //M420
  {ECHO_NOTIFY_TOAST, "echo:Active Extruder",    "알림:활성 익스트루더"},    //Tool Change
  {ECHO_NOTIFY_NONE,  "Unknown command: \"M150", "Unknown command: \"M150"},//M150
  {ECHO_NOTIFY_NONE,  "No SD card",              "SD카드 없음"},           //when using usb memory, prevent to pop up "No SD card"
  {ECHO_NOTIFY_NONE,  "SD card ok",              "SD 삽입"},    
  {ECHO_NOTIFY_NONE,  "Invalid mesh.",           "잘못된 메쉬 감지."},     //when using usb memory, prevent to pop up "SD card ok"
};

const ERROR_MSG knownError[] = {  
  {"Failed to enable Bed Leveling", "베드 레벨링 값이 없습니다."},
  {"volume.init failed",            "저장공간을 인식할 수 없습니다."},
  {"Probing Failed",                "Z 센서 감지 실패"},
  {"MAXTEMP triggered",             "최대 온도 초과"},
  {"MINTEMP triggered",             "최소 온도 미만"},
  {" cold extrusion prevented",     "저온 압출 방지"},
  {"Error writing to EEPROM!",      "EEPROM 저장중 오류 발생"},
  {"Hotend too cold",               "노즐 온도가 너무 낮습니다."},
  {"Heating failed",                "가열 불가 "},  
};

// uint8_t forceIgnore[ECHO_ID_COUNT] = {0};

void setCurrentAckSrc(uint8_t src)
{
  ack_cur_src = src;
  portSeen[src] = true;
}

static bool ack_seen(const char * str)
{
  int16_t str_len = strlen(str);
  int16_t max_len = dmaL2Cache_len - str_len;

  if(max_len < 0) // if str is longer than data present in cache, no match can be found
    return false;

  uint16_t i;
  for (ack_index = 0; ack_index <= max_len; ack_index++)
  {
    for (i = 0; i < str_len && dmaL2Cache[ack_index + i] == str[i]; i++)
    {}
    if(i == str_len) // if end of str is reached, a match was found
    {
      ack_index += i;
      return true;
    }
  }
  return false;
}

static bool ack_cmp(const char *str)
{
  uint16_t i;
  for(i=0; i < dmaL2Cache_len && str[i]!=0; i++)
  {
    if (str[i] != dmaL2Cache[i])
    return false;
  }
  if (str[i] != 0) return false;
  return true;
}

static float ack_value()
{
  return (strtod(&dmaL2Cache[ack_index], NULL));
}

// Read the value after the / if exists
static float ack_second_value()
{
  char *secondValue = strchr(&dmaL2Cache[ack_index],'/');
  if (secondValue != NULL)
  {
    return (strtod(secondValue+1, NULL));
  }
  else
  {
    return -0.5;
  }
}

void ack_values_sum(float *data)
{
  while (((dmaL2Cache[ack_index] < '0') || (dmaL2Cache[ack_index] > '9')) && dmaL2Cache[ack_index] != '\n')
    ack_index++;
  *data += ack_value();
  while ((((dmaL2Cache[ack_index] >= '0') && (dmaL2Cache[ack_index] <= '9')) || 
          (dmaL2Cache[ack_index] == '.'))  && (dmaL2Cache[ack_index] != '\n'))
    ack_index++;
  if (dmaL2Cache[ack_index] != '\n')
    ack_values_sum(data);
}

void ackPopupInfo(const char *info)
{
  bool show_dialog = true;
  if (infoMenu.menu[infoMenu.cur] == menuTerminal ||
      (infoMenu.menu[infoMenu.cur] == menuStatus && info == echomagic))
    show_dialog = false;

  // play notification sound if buzzer for ACK is enabled
  if (info == errormagic)
    BUZZER_PLAY(sound_error);
  else if (info == echomagic && infoSettings.ack_notification == 1)
    BUZZER_PLAY(sound_notify);

  // set echo message in status screen
  if (info == echomagic)
  {
    //ignore all messages if parameter settings is open
    if (infoMenu.menu[infoMenu.cur] == menuParameterSettings)
      return;

    //show notification based on notificaiton settings
    if (infoSettings.ack_notification == 1)
    {
      addNotification(DIALOG_TYPE_INFO, (char *)echoKr, (char *)dmaL2Cache + ack_index, show_dialog); //cremaker
    }
    else if (infoSettings.ack_notification == 2)
    {
      addToast(DIALOG_TYPE_INFO, dmaL2Cache);  //show toast notificaion if turned on
    }
  }
  else
  {
    addNotification(DIALOG_TYPE_ERROR, (char *)errorKr, (char *)dmaL2Cache + ack_index, show_dialog); //cremaker
  }
}

// void setIgnoreEcho(ECHO_ID msgId, bool state)
// {
//   forceIgnore[msgId] = state;
// }

bool processKnownErrorMsg(void)
{
  bool isKnown = false;
  uint8_t i;

  for(i = 0; i< COUNT(knownError); i++)
  {
    if(strstr(dmaL2Cache, knownError[i].msg))
    {
      isKnown = true;
      break;
    }
  }

  if(isKnown)
  {
    BUZZER_PLAY(sound_notify);
    addNotification(DIALOG_TYPE_ERROR, (char*)errormagicKr, (char*)knownError[i].msgKr + ack_index, true);
  }

  return isKnown;
}

bool processKnownEcho(void)
{
  bool isKnown = false;
  uint8_t i;

  for (i = 0; i < COUNT(knownEcho); i++)
  {
    if (strstr(dmaL2Cache, knownEcho[i].msg))
    {
      isKnown = true; 
      break;
    }
  }

  // display the busy indicator
  busyIndicator(STATUS_BUSY);

  if (isKnown)
  {
    if (knownEcho[i].notifyType == ECHO_NOTIFY_NONE)
      return isKnown;
    // if (forceIgnore[i] == 0)
    // {
      if (knownEcho[i].notifyType == ECHO_NOTIFY_TOAST)
        addToast(DIALOG_TYPE_INFO, dmaL2Cache);
      else if (knownEcho[i].notifyType == ECHO_NOTIFY_DIALOG)
      {
        BUZZER_PLAY(sound_notify);
        addNotification(DIALOG_TYPE_INFO, (char*)echomagicKr, (char*)knownEcho[i].msgKr + ack_index, true);
      }
    // }
  }
  return isKnown;
}

bool dmaL1NotEmpty(uint8_t port)
{
  return dmaL1Data[port].rIndex != dmaL1Data[port].wIndex;
}

void syncL2CacheFromL1(uint8_t port)
{
  uint16_t i = 0;
  while (dmaL1NotEmpty(port))
  {
    dmaL2Cache[i] = dmaL1Data[port].cache[dmaL1Data[port].rIndex];
    dmaL1Data[port].rIndex = (dmaL1Data[port].rIndex + 1) % dmaL1Data[port].cacheSize;
    if (dmaL2Cache[i++] == '\n') break;
  }
  dmaL2Cache_len = i; // length of data in the cache
  dmaL2Cache[i] = 0;  // End character
}

bool filterNoiseFromHost()
{  
  if(!infoPrinting.printing) return false;

  for (uint8_t i = 0; i < dmaL2Cache_len; i++)
  {
    if ((dmaL2Cache[i] & 0b10000000) == 0b10000000)
    {
      if(infoPrinting.ismoving)
      {
        infoHost.wait = false;
        infoPrinting.ismoving = false;
      }
      return true;
    }

    if (dmaL2Cache[i] == '\n')
    {
      if((i+1) == dmaL2Cache_len) break;
    }
  }

  if(!heatHasWaiting() && infoHost.wait != false && (OS_GetTimeMs()-sendCmdTime)>NOSIE_FILTER_TIME_INTERVAL)
  {
    if(!infoPrinting.ismoving && infoPrinting.progress != 100)
    {            
      infoHost.wait = false;

      return true;
    }
  }

  return false;
}

void hostActionCommands(void)
{
  char *find = strchr(dmaL2Cache + ack_index, '\n');
  *find = '\0';

  if (ack_seen("notification "))
  {
    strcpy(hostAction.prompt_begin, dmaL2Cache + ack_index);

    if(ack_seen("Settings Stored"))
    {
      statusScreen_setMsg((uint8_t *)echomagicKr, "설정 저장완료.");
    }
    else
    {
      statusScreen_setMsg((uint8_t *)echomagicKr, (uint8_t *)dmaL2Cache + ack_index);  
    }
  }

  if (ack_seen("prompt_begin "))
  {
    hostAction.button = 0;
    hostAction.prompt_show = 1;
    strcpy(hostAction.prompt_begin, dmaL2Cache + ack_index);
    if (ack_seen("Resuming SD"))  // print fron onboard SD starting
    {
      hostAction.prompt_show = 0;
      infoHost.printing = true;  // it's set by "File opened" but put here also just to be sure
      Serial_Puts(SERIAL_PORT, "M876 S0\n");  // auto-respond to a prompt request that is not shown on the TFT
    }
    else if (ack_seen("Resuming"))  //resuming from an onboard SD pause
    {
      infoPrinting.pause = false;
      infoHost.printing = true;
      hostAction.prompt_show = 0;
      Serial_Puts(SERIAL_PORT, "M876 S0\n");  // auto-respond to a prompt request that is not shown on the TFT
    }
    else if (ack_seen("Reheating"))
    {
      hostAction.prompt_show = 0;
      Serial_Puts(SERIAL_PORT, "M876 S0\n");  // auto-respond to a prompt request that is not shown on the TFT
    }
    else if (ack_seen("Nozzle Parked"))
    {
      infoPrinting.pause = true;
    }
  }
  else if (ack_seen("prompt_button "))
  {
    hostAction.button++;
    if (hostAction.button == 1)
    {
      strcpy(hostAction.prompt_button1, dmaL2Cache + ack_index);
    }
    else
    {
      strcpy(hostAction.prompt_button2, dmaL2Cache + ack_index);
    }
  }

  if (ack_seen("prompt_show") && hostAction.prompt_show)
  {
    switch(hostAction.button)
    {
      case 0:
        BUZZER_PLAY(sound_notify);
        setDialogText((uint8_t *)"Message", (uint8_t *)hostAction.prompt_begin, LABEL_CONFIRM,
                      LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_ALERT, setRunoutAlarmFalse, NULL, NULL);
        break;
      case 1:
        BUZZER_PLAY(sound_notify);
        setDialogText((uint8_t *)"Action command", (uint8_t *)hostAction.prompt_begin, (uint8_t *)hostAction.prompt_button1,
                      LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_ALERT, breakAndContinue, NULL, NULL);
        break;
      case 2:
        BUZZER_PLAY(sound_notify);
        setDialogText((uint8_t *)"Action command", (uint8_t *)hostAction.prompt_begin, (uint8_t *)hostAction.prompt_button1,
                      (uint8_t *)hostAction.prompt_button2);
        showDialog(DIALOG_TYPE_ALERT, resumeAndPurge, resumeAndContinue, NULL);
        break;
    }
  }

  if (ack_seen("paused") || ack_seen("pause"))
  {
    infoPrinting.pause = true;
    if (ack_seen ("filament_runout"))
    {
      setRunoutAlarmTrue();
    }
  }
  else if (ack_seen("cancel"))  //To be added to Marlin abortprint routine
  {
    if (infoHost.printing == true)
    {
      request_M27(0);
    }
    infoHost.printing = false;
    infoPrinting.printing = false;
    infoPrinting.cur = infoPrinting.size;
  }

}

void parseACK(void)
{
  if (infoHost.rx_ok[SERIAL_PORT] != true)
    return; //not get response data

  while (dmaL1NotEmpty(SERIAL_PORT))
  {
    bool avoid_terminal = false;
    syncL2CacheFromL1(SERIAL_PORT);
    infoHost.rx_ok[SERIAL_PORT] = false;
    if (infoHost.connected == false) // Not connected to printer
    {
      // parse error information even though not connected to printer
      if (ack_seen(errormagic))
        ackPopupInfo(errormagic);

      // the first response should be such as "T:25/50\n"
      if (!(ack_seen("@") && ack_seen("T:")) && !ack_seen("T0:"))
        goto parse_end;

      // find hotend count and setup heaters
      uint8_t i;
      for (i = NOZZLE0; i < MAX_HOTEND_COUNT; i++)
      {
        if (!ack_seen(heaterID[i]))
          break;
      }
      infoSettings.hotend_count = i ? i : 1;
      if (infoSettings.ext_count < infoSettings.hotend_count)
        infoSettings.ext_count = infoSettings.hotend_count;
      if (ack_seen(heaterID[BED]))
        infoSettings.bed_en = ENABLED;
      if (ack_seen(heaterID[CHAMBER]))
        infoSettings.chamber_en = ENABLED;
      updateNextHeatCheckTime();

      if (!ack_seen("@")) //It's RepRapFirmware
      {
        infoMachineSettings.firmwareType = FW_REPRAPFW;
        infoMachineSettings.softwareEndstops = ENABLED;
        infoHost.wait = false;
        storeCmd("M92\n");
        storeCmd("M115\n");
      }

      if (infoMachineSettings.firmwareType == FW_NOT_DETECTED) // if never connected to the printer since boot
      {
        storeCmd("M503\n"); // Query detailed printer capabilities
        storeCmd("M92\n");  // Steps/mm of extruder is an important parameter for Smart filament runout
                            // Avoid can't getting this parameter due to disabled M503 in Marlin
        storeCmd("M115\n");
        //storeCmd("M211\n"); // retrieve the software endstops state //cremaker
      }
      infoHost.connected = true;
    }

    // Onboard sd Gcode command response

    if (requestCommandInfo.inWaitResponse)
    {
      if (ack_seen(requestCommandInfo.startMagic))
      {
        requestCommandInfo.inResponse = true;
        requestCommandInfo.inWaitResponse = false;
      }
      else if ((requestCommandInfo.error_num > 0 && ack_seen(requestCommandInfo.errorMagic[0])) || (requestCommandInfo.error_num > 1 && ack_seen(requestCommandInfo.errorMagic[1])) || (requestCommandInfo.error_num > 2 && ack_seen(requestCommandInfo.errorMagic[2])))
      { // parse onboard sd error
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        requestCommandInfo.inError = true;
        requestCommandInfo.inWaitResponse = false;

        strcpy(requestCommandInfo.cmd_rev_buf, dmaL2Cache);
        BUZZER_PLAY(sound_error);
        goto parse_end;
      }
    }

    if (requestCommandInfo.inResponse)
    {
      if (strlen(requestCommandInfo.cmd_rev_buf) + strlen(dmaL2Cache) < CMD_MAX_REV)
      {
        strcat(requestCommandInfo.cmd_rev_buf, dmaL2Cache);
        if (ack_seen(requestCommandInfo.stopMagic))
        {
          requestCommandInfo.done = true;
          requestCommandInfo.inResponse = false;
        }
      }
      else
      {
        requestCommandInfo.done = true;
        requestCommandInfo.inResponse = false;
        ackPopupInfo(errormagic);
      }
      infoHost.wait = false;
      goto parse_end;
    }
    // Onboard sd Gcode command response end

    if (ack_cmp("ok\n"))
    {
      infoHost.wait = false;
      infoPrinting.ismoving = false;
    }
    else
    {
      if (ack_seen("ok"))
      {
        infoHost.wait = false;
        infoPrinting.ismoving = false;
      }

      // parse temperature
      if ((ack_seen("@") && ack_seen("T:")) || ack_seen("T0:"))
      {
        heatSetCurrentTemp(NOZZLE0, ack_value() + 0.5f);
        if (!heatGetSendWaiting(NOZZLE0))
          heatSyncTargetTemp(NOZZLE0, ack_second_value() + 0.5f);

        for (uint8_t i = 0; i < MAX_HEATER_COUNT; i++)
        {
          if (!heaterIsValid(i))
            continue;
          if (ack_seen(heaterID[i]))
          {
            heatSetCurrentTemp(i, ack_value() + 0.5f);
            if (!heatGetSendWaiting(i))
              heatSyncTargetTemp(i, ack_second_value() + 0.5f);
          }
        }

        if (ack_seen("P:"))
        {
          setProbeTemperature(ack_value());
        }

        avoid_terminal = !infoSettings.terminalACK;
        updateNextHeatCheckTime();
      }
      else if ((ack_seen("X:") && ack_index == 2) || ack_seen("C: X:")) // Smoothieware axis position starts with "C: X:"
      {
        coordinateSetAxisActual(X_AXIS, ack_value());
        if (ack_seen("Y:"))
        {
          coordinateSetAxisActual(Y_AXIS, ack_value());
          if (ack_seen("Z:"))
          {
            coordinateSetAxisActual(Z_AXIS, ack_value());
            if (ack_seen("E:"))
            {
              coordinateSetAxisActual(E_AXIS, ack_value());
            }
          }
        }
        coordinateQuerySetWait(false);
      }
      // Parse actual extruder position, response of "M114 E\n", required "M114_DETAIL" in Marlin
      else if (ack_seen("Count E:"))
      {
        coordinateSetExtruderActualSteps(ack_value());
      }
      // parse and store feed rate percentage
      else if ((infoMachineSettings.firmwareType == FW_REPRAPFW && ack_seen("factor: ")) ||
               ack_seen("FR:"))
      {
        speedSetCurPercent(0, ack_value());
        speedQuerySetWait(false);
      }
      // parse and store flow rate percentage
      else if (ack_seen("Flow: "))
      {
        speedSetCurPercent(1, ack_value());
        speedQuerySetWait(false);
      }
      // parse fan speed
      else if (ack_seen("M106 P"))
      {
        uint8_t i = ack_value();
        if (ack_seen("S"))
        {
          fanSetCurSpeed(i, ack_value());
        }
      }
      // parse controller fan
      else if (ack_seen("M710"))
      {
        uint8_t i = 0;
        if (ack_seen("S"))
        {
          i = fanGetTypID(0, FAN_TYPE_CTRL_S);
          fanSetCurSpeed(i, ack_value());
          fanQuerySetWait(false);
        }
        if (ack_seen("I"))
        {
          i = fanGetTypID(0, FAN_TYPE_CTRL_I);
          fanSetCurSpeed(i, ack_value());
          fanQuerySetWait(false);
        }
      }
      // parse "HOST_ACTION_COMMANDS"
      else if (ack_seen("//action:"))
      {
        hostActionCommands();
      }
      else if (infoMachineSettings.onboard_sd_support == ENABLED && ack_seen("File opened: "))
      {
        // File opened: 1A29A~1.GCO Size: 6974
        uint16_t start_index = ack_index;
        uint16_t end_index = ack_seen("Size: ") ? (ack_index - sizeof("Size: ")) : start_index;
        uint16_t path_len = MIN(end_index - start_index, MAX_PATH_LEN - strlen(getCurFileSource()) - 1);
        sprintf(infoFile.title, "%s/", getCurFileSource());
        strncat(infoFile.title, dmaL2Cache + start_index, path_len);
        infoFile.title[path_len + strlen(getCurFileSource()) + 1] = '\0';

        infoPrinting.pause = false;
        infoHost.printing = true;
        infoPrinting.time = 0;
        infoPrinting.cur = 0;
        infoPrinting.size = ack_value();

        infoFile.source = BOARD_SD_REMOTE;
        initPrintSummary();
        infoMenu.cur = 1; // take care if popup active or user in other menu than print
        infoMenu.menu[infoMenu.cur] = menuPrinting;

        if (infoMachineSettings.autoReportSDStatus == 1)
        {
          request_M27(infoSettings.m27_refresh_time); //Check if there is a SD or USB print running.
        }
      }
      else if (ack_seen("Not SD printing"))
      {
        if (infoMachineSettings.onboard_sd_support == ENABLED && infoFile.source >= BOARD_SD)
        {
          infoHost.printing = false;
          if (infoPrinting.printing)
            infoPrinting.pause = true;
        }
      }
      else if (infoMachineSettings.onboard_sd_support == ENABLED &&
               infoFile.source >= BOARD_SD &&
               ack_seen("SD printing byte"))
      {
        infoPrinting.pause = false;
        // Parsing printing data
        // Example: SD printing byte 123/12345
        infoPrinting.cur = ack_value();
        // powerFailedCache(position);
      }
      else if (infoMachineSettings.onboard_sd_support == ENABLED &&
               infoFile.source >= BOARD_SD &&
               ack_seen("Done printing file"))
      {
        infoHost.printing = false;
        printingFinished();
        infoPrinting.cur = infoPrinting.size;
      }

      // parse and store stepper steps/mm values
      else if (ack_seen("M92"))
      {
        if (ack_seen("M92 X"))
        {
          setParameter(P_STEPS_PER_MM, X_STEPPER, ack_value());
          if (ack_seen("Y"))
            setParameter(P_STEPS_PER_MM, Y_STEPPER, ack_value());
          if (ack_seen("Z"))
            setParameter(P_STEPS_PER_MM, Z_STEPPER, ack_value());
        }
        if (ack_seen("T"))
        {
          if (ack_seen("T0 E"))
            setParameter(P_STEPS_PER_MM, E_STEPPER, ack_value());
          if (ack_seen("T1 E"))
          {
            setParameter(P_STEPS_PER_MM, E2_STEPPER, ack_value());
            setDualStepperStatus(E_AXIS, true);
          }
        }
        else if (ack_seen("E"))
        {
          setParameter(P_STEPS_PER_MM, E_STEPPER, ack_value());
        }
      }
      // parse and store stepper steps/mm values incase of RepRapFirmware
      else if ((infoMachineSettings.firmwareType == FW_REPRAPFW) && (ack_seen("Steps")))
      {
        if (ack_seen("X: "))
          setParameter(P_STEPS_PER_MM, X_STEPPER, ack_value());
        if (ack_seen("Y: "))
          setParameter(P_STEPS_PER_MM, Y_STEPPER, ack_value());
        if (ack_seen("Z: "))
          setParameter(P_STEPS_PER_MM, Z_STEPPER, ack_value());
        if (ack_seen("E: "))
          setParameter(P_STEPS_PER_MM, E_STEPPER, ack_value());
      }
      // parse and store Max Acceleration values
      else if (ack_seen("M201"))
      {
        if (ack_seen("M201 X"))
        {
          setParameter(P_MAX_ACCELERATION, X_STEPPER, ack_value());
          if (ack_seen("Y"))
            setParameter(P_MAX_ACCELERATION, Y_STEPPER, ack_value());
          if (ack_seen("Z"))
            setParameter(P_MAX_ACCELERATION, Z_STEPPER, ack_value());
        }
        if (ack_seen("T"))
        {
          if (ack_seen("T0 E"))
            setParameter(P_MAX_ACCELERATION, E_STEPPER, ack_value());
          if (ack_seen("T1 E"))
          {
            setParameter(P_MAX_ACCELERATION, E2_STEPPER, ack_value());
            setDualStepperStatus(E_AXIS, true);
          }
        }
        else if (ack_seen("E"))
        {
          setParameter(P_MAX_ACCELERATION, E_STEPPER, ack_value());
        }
      }
      // parse and store Max Feed Rate values
      else if (ack_seen("M203"))
      {
        if (ack_seen("M203 X"))
        {
          setParameter(P_MAX_FEED_RATE, X_STEPPER, ack_value());
          if (ack_seen("Y"))
            setParameter(P_MAX_FEED_RATE, Y_STEPPER, ack_value());
          if (ack_seen("Z"))
            setParameter(P_MAX_FEED_RATE, Z_STEPPER, ack_value());
        }
        if (ack_seen("T"))
        {
          if (ack_seen("T0 E"))
            setParameter(P_MAX_FEED_RATE, E_STEPPER, ack_value());
          if (ack_seen("T1 E"))
          {
            setParameter(P_MAX_FEED_RATE, E2_STEPPER, ack_value());
            setDualStepperStatus(E_AXIS, true);
          }
        }
        else if (ack_seen("E"))
        {
          setParameter(P_MAX_FEED_RATE, E_STEPPER, ack_value());
        }
      }
      // parse and store Acceleration values
      else if (ack_seen("M204 P"))
      {
        setParameter(P_ACCELERATION, 0, ack_value());
        if (ack_seen("R"))
          setParameter(P_ACCELERATION, 1, ack_value());
        if (ack_seen("T"))
          setParameter(P_ACCELERATION, 2, ack_value());
      }
      // parse and store jerk values
      else if (ack_seen("M205"))
      {
        if (ack_seen("X"))
          setParameter(P_JERK, X_STEPPER, ack_value());
        if (ack_seen("Y"))
          setParameter(P_JERK, Y_STEPPER, ack_value());
        if (ack_seen("Z"))
          setParameter(P_JERK, Z_STEPPER, ack_value());
        if (ack_seen("E"))
          setParameter(P_JERK, E_STEPPER, ack_value());
        if (ack_seen("J"))
          setParameter(P_JUNCTION_DEVIATION, 0, ack_value());
      }
      // parse and store Home Offset values
      else if (ack_seen("M206 X"))
      {
        setParameter(P_HOME_OFFSET, X_STEPPER, ack_value());
        if (ack_seen("Y"))
          setParameter(P_HOME_OFFSET, Y_STEPPER, ack_value());
        if (ack_seen("Z"))
          setParameter(P_HOME_OFFSET, Z_STEPPER, ack_value());
      }
      // parse and store FW retraction values
      else if (ack_seen("M207 S"))
      {
        setParameter(P_FWRETRACT, 0, ack_value());
        if (ack_seen("W"))
          setParameter(P_FWRETRACT, 1, ack_value());
        if (ack_seen("F"))
          setParameter(P_FWRETRACT, 2, ack_value());
        if (ack_seen("Z"))
          setParameter(P_FWRETRACT, 3, ack_value());
      }
      // parse and store FW recover values
      else if (ack_seen("M208 S"))
      {
        setParameter(P_FWRECOVER, 0, ack_value());
        if (ack_seen("W"))
          setParameter(P_FWRECOVER, 1, ack_value());
        if (ack_seen("F"))
          setParameter(P_FWRECOVER, 2, ack_value());
        if (ack_seen("R"))
          setParameter(P_FWRECOVER, 3, ack_value());
      }
      // parse and store auto FW retract state (M209 - Set Auto Retract)
      else if (ack_seen("M209 S"))
      {
        setParameter(P_AUTO_RETRACT, 0, ack_value());
      }
      // parse and store the software endstops state (M211)
      else if (ack_seen("Soft endstops"))
      {
        uint8_t curValue = infoMachineSettings.softwareEndstops;
        infoMachineSettings.softwareEndstops = ack_seen("ON");

        if (curValue != infoMachineSettings.softwareEndstops) // send a notification only if status is changed
          addToast(DIALOG_TYPE_INFO, dmaL2Cache);
      }
      // parse and store Offset 2nd Nozzle
      else if (ack_seen("M218 T1 X"))
      {
        setParameter(P_HOTEND_OFFSET, 0, ack_value());
        if (ack_seen("Y"))
          setParameter(P_HOTEND_OFFSET, 1, ack_value());
        if (ack_seen("Z"))
          setParameter(P_HOTEND_OFFSET, 2, ack_value());
      }
      // parse and store ABL on/off state & Z fade value on M503
      else if (ack_seen("M420 S"))
      {
        if (ack_seen("S"))
          setParameter(P_ABL_STATE, 0, ack_value());
        if (ack_seen("Z"))
          setParameter(P_ABL_STATE, 1, ack_value());
      }
      // parse and store Probe Offset values
      else if (ack_seen("M851 X"))
      {
        setParameter(P_PROBE_OFFSET, X_STEPPER, ack_value());
        if (ack_seen("Y"))
          setParameter(P_PROBE_OFFSET, Y_STEPPER, ack_value());
        if (ack_seen("Z"))
          setParameter(P_PROBE_OFFSET, Z_STEPPER, ack_value());
      }
      // parse and store linear advance values
      else if (ack_seen("M900"))
      {
        if (ack_seen("T"))
        {
          if (ack_seen("T0 K"))
            setParameter(P_LIN_ADV, 0, ack_value());
          if (ack_seen("T1 K"))
            setParameter(P_LIN_ADV, 1, ack_value());
        }
        else if (ack_seen("K"))
        {
          setParameter(P_LIN_ADV, 0, ack_value());
        }
      }
      // parse and store stepper driver current values
      else if (ack_seen("M906"))
      {
        if (ack_seen("I1"))
        {
          if (ack_seen("X"))
            setDualStepperStatus(X_AXIS, true);
          if (ack_seen("Y"))
            setDualStepperStatus(Y_AXIS, true);
          if (ack_seen("Z"))
            setDualStepperStatus(Z_AXIS, true);
        }
        else
        {
          if (ack_seen("X"))
            setParameter(P_CURRENT, X_STEPPER, ack_value());
          if (ack_seen("Y"))
            setParameter(P_CURRENT, Y_STEPPER, ack_value());
          if (ack_seen("Z"))
            setParameter(P_CURRENT, Z_STEPPER, ack_value());
        }
        if (ack_seen("T"))
        {
          if (ack_seen("T0 E"))
            setParameter(P_CURRENT, E_STEPPER, ack_value());
          if (ack_seen("T1 E"))
          {
            setParameter(P_CURRENT, E2_STEPPER, ack_value());
            setDualStepperStatus(E_AXIS, true);
          }
        }
        else if (ack_seen("E"))
        {
          setParameter(P_CURRENT, E_STEPPER, ack_value());
        }
      }
      // parse and store TMC Hybrid Threshold Speed
      else if (ack_seen("M913"))
      {
        if (ack_seen("M913 X"))
        {
          setParameter(P_HYBRID_THRESHOLD, X_STEPPER, ack_value());
          if (ack_seen("Y"))
            setParameter(P_HYBRID_THRESHOLD, Y_STEPPER, ack_value());
          if (ack_seen("Z"))
            setParameter(P_HYBRID_THRESHOLD, Z_STEPPER, ack_value());
        }
        if (ack_seen("T"))
        {
          if (ack_seen("T0 E"))
            setParameter(P_HYBRID_THRESHOLD, E_STEPPER, ack_value());
          if (ack_seen("T1 E"))
          {
            setParameter(P_HYBRID_THRESHOLD, E2_STEPPER, ack_value());
            setDualStepperStatus(E_AXIS, true);
          }
        }
        else if (ack_seen("E"))
        {
          setParameter(P_HYBRID_THRESHOLD, E_STEPPER, ack_value());
        }
      }
      // parse and store TMC Bump sensitivity values
      else if (ack_seen("M914 X"))
      {
        setParameter(P_BUMPSENSITIVITY, X_STEPPER, ack_value());
        if (ack_seen("Y"))
          setParameter(P_BUMPSENSITIVITY, Y_STEPPER, ack_value());
        if (ack_seen("Z"))
          setParameter(P_BUMPSENSITIVITY, Z_STEPPER, ack_value());
      }
// parse and store ABL type if auto-detect is enabled
#if ENABLE_BL_VALUE == 1
      else if (ack_seen("Auto Bed Leveling"))
        infoMachineSettings.leveling = BL_ABL;
      else if (ack_seen("Unified Bed Leveling"))
        infoMachineSettings.leveling = BL_UBL;
      else if (ack_seen("Mesh Bed Leveling"))
        infoMachineSettings.leveling = BL_MBL;
#endif
      // parse ABL state (M420)
      else if (ack_seen("echo:Bed Leveling"))
      {
        if (ack_seen("ON"))
          setParameter(P_ABL_STATE, 0, ENABLED);
        else
          setParameter(P_ABL_STATE, 0, DISABLED);
      }
      // parse M115 capability report
      else if (ack_seen("FIRMWARE_NAME:"))
      {
        uint8_t *string = (uint8_t *)&dmaL2Cache[ack_index];
        uint16_t string_start = ack_index;
        uint16_t string_end = string_start;

        if (ack_seen("Marlin"))
        {
          infoMachineSettings.firmwareType = FW_MARLIN;
        }
        else if (ack_seen("RepRapFirmware"))
        {
          infoMachineSettings.firmwareType = FW_REPRAPFW;
          setupMachine();
        }
        else
        {
          infoMachineSettings.firmwareType = FW_UNKNOWN;
          setupMachine();
        }
        if (ack_seen("FIRMWARE_URL:")) // For Smoothieware
          string_end = ack_index - sizeof("FIRMWARE_URL:");
        else if (ack_seen("SOURCE_CODE_URL:")) // For Marlin
          string_end = ack_index - sizeof("SOURCE_CODE_URL:");
        else if ((infoMachineSettings.firmwareType == FW_REPRAPFW) && ack_seen("ELECTRONICS")) // For RepRapFirmware
          string_end = ack_index - sizeof("ELECTRONICS");

        infoSetFirmwareName(string, string_end - string_start); // Set firmware name

        if (ack_seen("MACHINE_TYPE:"))
        {
          string = (uint8_t *)&dmaL2Cache[ack_index];
          string_start = ack_index;
          if (ack_seen("EXTRUDER_COUNT:"))
          {
            if (MIXING_EXTRUDER == 0)
            {
              infoSettings.ext_count = ack_value();
            }
            string_end = ack_index - sizeof("EXTRUDER_COUNT:");
          }
          infoSetMachineType(string, string_end - string_start); // Set firmware name
        }
      }
      else if (ack_seen("Cap:EEPROM:"))
      {
        infoMachineSettings.EEPROM = ack_value();
      }
      else if (ack_seen("Cap:AUTOREPORT_TEMP:"))
      {
        infoMachineSettings.autoReportTemp = ack_value();
        if (infoMachineSettings.autoReportTemp)
        {
          storeCmd("M155 ");
        }
      }
      else if (ack_seen("Cap:AUTOLEVEL:") && infoMachineSettings.leveling == BL_DISABLED)
      {
        infoMachineSettings.leveling = ack_value();
      }
      else if (ack_seen("Cap:Z_PROBE:"))
      {
        infoMachineSettings.zProbe = ack_value();
      }
      else if (ack_seen("Cap:LEVELING_DATA:"))
      {
        infoMachineSettings.levelingData = ack_value();
      }
      else if (ack_seen("Cap:SOFTWARE_POWER:"))
      {
        infoMachineSettings.softwarePower = ack_value();
      }
      else if (ack_seen("Cap:TOGGLE_LIGHTS:"))
      {
        infoMachineSettings.toggleLights = ack_value();
      }
      else if (ack_seen("Cap:CASE_LIGHT_BRIGHTNESS:"))
      {
        infoMachineSettings.caseLightsBrightness = ack_value();
      }
      else if (ack_seen("Cap:EMERGENCY_PARSER:"))
      {
        infoMachineSettings.emergencyParser = ack_value();
      }
      else if (ack_seen("Cap:PROMPT_SUPPORT:"))
      {
        infoMachineSettings.promptSupport = ack_value();
      }
      else if (ack_seen("Cap:SDCARD:") && infoSettings.onboardSD == AUTO)
      {
        infoMachineSettings.onboard_sd_support = ack_value();
      }
      else if (ack_seen("Cap:AUTOREPORT_SD_STATUS:"))
      {
        infoMachineSettings.autoReportSDStatus = ack_value();
      }
      else if (ack_seen("Cap:LONG_FILENAME:") && infoSettings.longFileName == AUTO)
      {
        infoMachineSettings.long_filename_support = ack_value();
      }
      else if (ack_seen("Cap:BABYSTEPPING:"))
      {
        infoMachineSettings.babyStepping = ack_value();
      }
      else if (ack_seen("Cap:CHAMBER_TEMPERATURE:"))
      {
        infoSettings.chamber_en = ack_value();
        setupMachine();
      }
      else if (ack_seen("work:"))
      {
        if (ack_seen("min:"))
        {
          if (ack_seen("X:"))
            infoSettings.machine_size_min[X_AXIS] = ack_value();
          if (ack_seen("Y:"))
            infoSettings.machine_size_min[Y_AXIS] = ack_value();
          if (ack_seen("Z:"))
            infoSettings.machine_size_min[Z_AXIS] = ack_value();
        }
        if (ack_seen("max:"))
        {
          if (ack_seen("X:"))
            infoSettings.machine_size_min[X_AXIS] = ack_value();
          if (ack_seen("Y:"))
            infoSettings.machine_size_min[Y_AXIS] = ack_value();
          if (ack_seen("Z:"))
            infoSettings.machine_size_min[Z_AXIS] = ack_value();
        }
      }
      // parse Repeatability Test
      else if (ack_seen("Mean:"))
      {
        char tmpMsg[100];
        strcpy(tmpMsg, "Mean: ");
        sprintf(&tmpMsg[strlen(tmpMsg)], "%0.5f", ack_value());
        if (ack_seen("Min: "))
        {
          sprintf(&tmpMsg[strlen(tmpMsg)], "\nMin: %0.5f", ack_value());
        }
        if (ack_seen("Max: "))
        {
          sprintf(&tmpMsg[strlen(tmpMsg)], "\nMax: %0.5f", ack_value());
        }
        if (ack_seen("Range: "))
        {
          sprintf(&tmpMsg[strlen(tmpMsg)], "\nRange: %0.5f", ack_value());
        }
        setDialogText((u8 *)"Repeatability Test", (uint8_t *)tmpMsg, LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
      }
      else if (ack_seen("Standard Deviation: "))
      {
        char tmpMsg[100];
        strncpy(tmpMsg, (char *)getDialogMsgStr(), 6);
        tmpMsg[6] = '\0';
        if (strcmp(tmpMsg, "Mean: ") == 0)
        {
          sprintf(tmpMsg, "%s\nStandard Deviation: %0.5f", (char *)getDialogMsgStr(), ack_value());
          setDialogText((u8 *)"Repeatability Test", (uint8_t *)tmpMsg, LABEL_CONFIRM, LABEL_BACKGROUND);
          showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
        }
      }
      // parse and store Probe Z offset value (M851)
      else if (ack_seen("Probe Offset"))
      {
        if (ack_seen("Z:") || (ack_seen("Z")))
        {
          setParameter(P_PROBE_OFFSET, Z_STEPPER, ack_value());
        }
      }
      // parse and store MBL Z offset value (from M503 (G29 S4 Zxx) or M420 V1 T1)
      else if (ack_seen("G29 S4 Z") || ack_seen("mesh. Z offset:"))
      {
        setParameter(P_MBL_OFFSET, 0, ack_value());
      }
      // parse and store flow rate percentage incase of RepRapFirmware
      else if ((infoMachineSettings.firmwareType == FW_REPRAPFW) && ack_seen("extruder"))
      {
        ack_index += 4;
        speedSetCurPercent(1, ack_value());
        speedQuerySetWait(false);
      }
      else if (ack_seen("Case light: OFF"))
      {
        caseLightSetState(false);
        caseLightQuerySetWait(false);
      }
      else if (ack_seen("Case light: "))
      {
        caseLightSetState(true);
        caseLightSetBrightness(ack_value());
        caseLightQuerySetWait(false);
      }
      // parse pause message
      else if (!infoMachineSettings.promptSupport && ack_seen("paused for user"))
      {
        setDialogText((u8 *)"Printer is Paused", (u8 *)"Paused for user\ncontinue?", LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_QUESTION, breakAndContinue, NULL, NULL);
      }
      // parse ABL Complete message
      else if (ack_seen("ABL Complete"))
      {
        ablUpdateStatus(true);
      }
      // parse BBL Complete message
      else if (ack_seen("BBL Complete"))
      {
        ablUpdateStatus(true);
      }
      // parse UBL Complete message
      else if (ack_seen("UBL Complete"))
      {
        ablUpdateStatus(true);
      }
      // parse MBL Complete message
      else if (ack_seen("Mesh probing done"))
      {
        mblUpdateStatus(true);
      }
      // parse Mesh data
      else if (meshIsWaitingFirstData() && (ack_seen("Mesh Bed Level data:") ||           // MBL
                                            ack_seen("Bed Topography Report for CSV:") || // UBL
                                            ack_seen("Bilinear Leveling Grid:") ||        // ABL Bilinear
                                            ack_seen("Bed Level Correction Matrix:") ||   // ABL Linear or 3-Point
                                            ack_seen("Invalid mesh")))                    // error echo
      {
        meshUpdateData(dmaL2Cache); // start data updating
      }
      else if (ack_seen("Bilinear Leveling Grid:"))
      {
        didLeveling = true; //cremaker
      }
      else if (meshIsWaitingData())
      {
        meshUpdateData(dmaL2Cache); // continue data updating
      }
      // parse PID Autotune finished message
      else if (ack_seen("PID Autotune finished"))
      {
        pidUpdateStatus(true);
      }
      // parse PID Autotune failed message
      else if (ack_seen("PID Autotune failed"))
      {
        pidUpdateStatus(false);
      }
      // parse filament data from gCode (M118)
      else if (ack_seen("filament_data"))
      {
        if (ack_seen("L:"))
        {
          ack_values_sum(&infoPrintSummary.length);
        }
        else if (ack_seen("W:"))
        {
          ack_values_sum(&infoPrintSummary.weight);
        }
        else if (ack_seen("C:"))
        {
          ack_values_sum(&infoPrintSummary.cost);
        }
        hasFilamentData = true;
      }
      // belt tenstion test finished
      else if (ack_seen("belt test"))
      {
        char tempstr[70] = "XY 테스트가 완료되었습니다.\n";
        char tempstrX[20];
        char tempstrY[20];

        BUZZER_PLAY(sound_error);

        if (ack_seen("X"))
        {
          sprintf(tempstrX, "%.2f", ack_value());
          strcat(tempstr, " X:");
          strcat(tempstr, tempstrX);
        }
        if (ack_seen("Y"))
        {
          sprintf(tempstrY, "%.2f", ack_value());
          strcat(tempstr, " Y:");
          strcat(tempstr, tempstrY);
        }

        if (infoSettings.ack_notification == 1)
        {
          addNotification(DIALOG_TYPE_INFO, "테스트 완료", tempstr, true);
        }
        storeCmd("M84\n");
      }
      else if (ack_seen("SETPPER_SEN"))
      {
        char tempStr[50];

        if (ack_seen("X"))
        {
          strcat(tempStr, "X, ");
        }
        if (ack_seen("Y"))
        {
          strcat(tempStr, "Y ");
        }

        if (infoSettings.ack_notification == 1)
        {
          strcat(tempStr, "축 감도 조절하세요.!");
          addNotification(DIALOG_TYPE_INFO, "모터 감도 조절 필요!", (char *)tempStr, true);
        }
      }
      // x,y wiring test
      else if (ack_seen("DIW XY"))
      {
        BUZZER_PLAY(sound_error);

        if (infoSettings.ack_notification == 1)
        {
          addNotification(DIALOG_TYPE_INFO, "배선 점검 필요!", "잘못된 배선이 감지되었습니다. 배선을 확인하세요.", true);
        }
      }
      // in belt testing, detect X loose belt
      else if (ack_seen("Blet loose"))
      {
        BUZZER_PLAY(sound_error);
        char tempstr[150] = "벨트가 느슨한지 확인하세요.\n벨트 장력을 조절하세요.\n";
        char tempstrlen[50];
        float xlenf, ylenf;

        if (ack_seen("X"))
        {
          xlenf = ack_value();
          sprintf(tempstrlen, "X : %.3f ", xlenf);
          strcat(tempstr, tempstrlen);
        }

        if (ack_seen("Y"))
        {
          ylenf = ack_value();
          sprintf(tempstrlen, "Y : %.3f", xlenf);
          strcat(tempstr, tempstrlen);
        }

        setDialogText(LABEL_INFO, (uint8_t *)tempstr, LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
        storeCmd("M84\n");
      }
      // for live z offset control cmd
      else if (ack_seen("L offset Z"))
      {
        setParameter(P_PROBE_OFFSET, Z_STEPPER, ack_value());
      }
      // to get tmc2209 sgresult value
      else if (ack_seen("SGRESULT"))
      {
        if (!isTestingFilaExt)
        {
          isTestingFilaExt = true;
        }
        float sgValue = ack_value();
        setSgrValue(sgValue);

        if (sgValue > SGR_RESULT_MAX || (sgValue < SGR_RESULT_MIN) && (sgValue > SGR_RESULT_NOISE))
          increaseDetectSgrValue();
      }
      // finished load filament test
      else if (ack_seen("LOAD_CHECK_FINISH"))
      {
        addToast(DIALOG_TYPE_INFO, "토출 테스트 완료.");
      }
      // finish extruding filament
      else if (ack_seen("FEF"))
      {
        char finishedStr[50] = "삽입이 완료되었습니다.";
        setDialogText(LABEL_INFO, (uint8_t *)finishedStr, LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
      }
      // finish removing filament
      else if (ack_seen("FRF"))
      {
        char finishedStr[50] = "제거가 완료되었습니다.";
        setDialogText(LABEL_INFO, (uint8_t *)finishedStr, LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
      }
      // check currnet filament weight value
      else if (ack_seen("Cur Fil W :"))
      {
        filaweight.remainFilatWeight = ack_value();
      }
      // check total used filament weight value
      else if (ack_seen("Total used Fil W :"))
      {
        filaweight.totalUsedFilaWeight = ack_value();
      }
      // to get filament density value
      else if (ack_seen("Fila Density :"))
      {
        filaweight.filaDensity = ack_value();
      }
      // finish probe calibration
      else if (ack_seen("FPCG76"))
      {
        infoTesting.testing = false;
        infoTesting.progress++;
        GUI_ClearPrect(&screen_without_Right);
      }
      // average measured value of 4Point bed leveling test
      else if (ack_seen("AVG_MEA_Z:"))
      {
        inclineLevelCali.avg_z_value = ack_value();
        isChangedValue = true;
      }
      // to get measured value left front of 4Point bed leveling control cmd
      else if (ack_seen("MEA_Z_LF:"))
      {
        inclineLevelCali.lf_z_value = ack_value();
        isChangedValue = true;
      }
      // to get measured value right front of 4Point bed leveling control cmd
      else if (ack_seen("MEA_Z_RF:"))
      {
        inclineLevelCali.rf_z_value = ack_value();
        isChangedValue = true;
      }
      // to get measured value right back of 4Point bed leveling control cmd
      else if (ack_seen("MEA_Z_RB:"))
      {
        inclineLevelCali.rb_z_value = ack_value();
        isChangedValue = true;
      }
      // to get measured value left back of 4Point bed leveling control cmd
      else if (ack_seen("MEA_Z_LB:"))
      {
        inclineLevelCali.lb_z_value = ack_value();
        isChangedValue = true;
      }
      // z horizonal calibration was finished
      else if (ack_seen("ZHC FINISH"))
      {
        plusTestingProgress();
      }
      // value of leveling grid max x
      else if (ack_seen("GMPX"))
      {
        addZOffsetCal.colN = ack_value();
      }
      // value of leveling grid max y
      else if (ack_seen("GMPY"))
      {
        addZOffsetCal.rowN = ack_value();
      }
      // add value to z offset for probing calibration
      else if (ack_seen("AD_VAL"))
      {
        if (ack_seen("I"))
        {
          addZOffsetCal.colindex = ack_value();
        }
        if (ack_seen("J"))
        {
          addZOffsetCal.rowindex = (addZOffsetCal.rowN - 1) - ack_value(); //invert row 1~n to row n~1
        }
        if (ack_seen("Z"))
        {
          addZOffsetCal.add_z_value[addZOffsetCal.colindex][addZOffsetCal.rowindex] = ack_value();
        }
        if (ack_seen("MAX"))
        {
          addZOffsetCal.max_add_z_value = ack_value();
        }
        if (ack_seen("MIN"))
        {
          addZOffsetCal.min_add_z_value = ack_value();
          addZOffsetCal.isNeedRedraw = true;
        }
      }
      // set probe temperature calibration for winter
      else if (ack_seen("PTC_WINTER"))
      {
        if (seasonValue != SEASON_WINTER)
        {
          addToast(DIALOG_TYPE_ERROR, "계절 값이 다릅니다.");
        }
      }
      // set probe temperature calibration for not winter
      else if (ack_seen("PTC_SUMMER"))
      {
        if (seasonValue != SEASON_SUMMER)
        {
          addToast(DIALOG_TYPE_ERROR, "계절 값이 다릅니다.");
        }
      }
      else if (ack_seen("Failed to enable Bed Leveling"))
      {
        char noDataOfLeveling[75] = "베드 레벨링 값을 불러올 수 없습니다.";
        setDialogText(LABEL_INFO, (uint8_t *)noDataOfLeveling, LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
      }
      else if (ack_seen("Advance K="))
      {
        valueK = ack_value();
        isRecValueK = true;
      }
      else if (ack_seen("!Probe heating timed out."))
      {
        char failedCalProbe[75] = "프로브 조정에 실패했습니다.";
        setDialogText(LABEL_INFO, (uint8_t *)failedCalProbe, LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);
        finishTesting();
      }
      // auto printing is enable state
      else if (ack_seen("APSD"))
      {
        if (ack_seen("L"))
        {
          rightOrLeft = true;
        }
        else if (ack_seen("R"))
        {
          rightOrLeft = false;
        }

        isAutoPrinting = true;
      }
      // auto printing is disable state
      else if (ack_seen("NAPSD"))
      {
        isAutoPrinting = false;
        addToast(DIALOG_TYPE_INFO, "3");
      }
      else if (ack_seen("FIRST_PRINT"))
      {
        infoFile.source = BOARD_SD;
        scanPrintFiles();
        infoFile.fileIndex = 0;
        sprintf(infoFile.title, getCurFileSource());
        strcat(infoFile.title, "/");
        strcat(infoFile.title, (uint8_t *)infoFile.file[infoFile.fileIndex]);
        startPrint();
        purgeLastCmd();
      }
      // parse error messages & Echo messages
      else if (ack_seen(errormagic))
      {
        if (!processKnownErrorMsg())
          ackPopupInfo(errormagic);
      }
      // if no known echo was found and processed, then popup the echo message
      else if (ack_seen(echomagic))
      {
        if (!processKnownEcho())
          ackPopupInfo(echomagic);
      }
      else
      {
        if (filterNoiseFromHost())
          return;
      }

    parse_end:
      if (ack_cur_src != SERIAL_PORT)
      {
        Serial_Puts(ack_cur_src, dmaL2Cache);
      }
      else if (!ack_seen("ok") || ack_seen("T:") || ack_seen("T0:"))
      {
        // make sure we pass on spontaneous messages to all connected ports (since these can come unrequested)
        for (SERIAL_PORT_INDEX i = PORT_2; i < SERIAL_PORT_COUNT; i++)
        {
          if (infoSettings.port_on[i]) // if serial port is enabled
          {
            if (portSeen[portNum[i]])
            {
              // pass on this one to anyone else who might be listening
              Serial_Puts(portNum[i], dmaL2Cache);
            }
          }
        }
      }

      if (avoid_terminal != true)
      {
        if (!ack_cmp("Not SD printing\n")) //debug test
        {
          sendGcodeTerminalCache(dmaL2Cache, TERMINAL_ACK);
        }
      }
    }
  }
}

void parseRcvGcode(void)
{
  #ifdef SERIAL_PORT_2
    for (SERIAL_PORT_INDEX i = PORT_2; i < SERIAL_PORT_COUNT; i++)  // scan all the supplementary serial ports
    {
      if (infoSettings.port_on[i])  // if serial port is enabled
      {
        if (infoHost.rx_ok[portNum[i]] == true)
        {
          infoHost.rx_ok[portNum[i]] = false;
          while(dmaL1NotEmpty(portNum[i]))
          {
            syncL2CacheFromL1(portNum[i]);
            storeCmdFromUART(portNum[i], dmaL2Cache, NO_TFT_DISK);
          }
        }
      }
    }
  #endif
}
