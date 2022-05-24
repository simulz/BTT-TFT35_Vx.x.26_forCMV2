#include "ReadyCheck.h"

TESTING infoTesting;

#define BED_PREHEAT_TIME 5
#define NOZZLE_PREHEAT_TIME 3

static uint8_t curExtruder_index = 0;
const static uint8_t nozzleTestTemp    = 200;
const static uint8_t bedTestTemp       = 60;

static uint32_t preheatBeginTime = 0;
static uint32_t preheatCurTime   = 0;
static uint32_t totalminTime     = 0;
static uint32_t totalsecTime     = 0;

static uint32_t nozzleResultTempMinTime     = 0;
static uint32_t nozzleResultTempSecTime     = 0;
static uint32_t bedResultTempMinTime        = 0;
static uint32_t bedResultTempSecTime        = 0;

// Start testing progress
void startTesting(void)
{
  infoTesting.testing = true;
}

// finish testing progress
void finishTesting(void)
{
  infoTesting.testing = false;
}

// testing progress Num ++
void plusTestingProgress(void)
{
  infoTesting.progress++;
}

// testing progress Num --
void minusTestingProgress(void)
{
  infoTesting.progress--;
}

// get current progress Num
uint16_t getTestingProgress(void)
{
  return infoTesting.progress;
}

// set test progress num to num
void setTestingProgress(uint16_t num)
{
  infoTesting.progress = num;
}

// set test progress num to zero
void setZeroTestingProgress()
{
  setTestingProgress(0);
}

//turn off cooling fan in extruder
void offCoolingFan(void)
{
  storeCmd("M106 S0\n");
}

//popup to check cooling fan
void checkCoolingFan(void)
{
  char noteCheckCoolingFan[140]  = "익스트루더 정면의 보조팬이 작동하는지 확인하세요. 확인 후 확인 버튼을 누르세요";
  storeCmd("M103\n");
  storeCmd("M106 S255\n");

  setDialogText(LABEL_CHECK, (uint8_t *)noteCheckCoolingFan, LABEL_CONFIRM, LABEL_CANCEL);
  showDialog(DIALOG_TYPE_INFO, offCoolingFan, NULL, NULL);
}

// when fan do not work and user push cancel Btn, working-fucntion
void failToCheckCoolingFan(void)
{
  storeCmd("M103\n");
}

//popup to check main fan
void checkMainFan(void)
{
  char noteCheckMainFan[140]     = "익스트루더 왼편의 메인팬(쿨링)이 작동하는지 확인하세요. 확인 후 다음 버튼을 누르세요";

  setDialogText(LABEL_CHECK, (uint8_t *)noteCheckMainFan, LABEL_NEXT, LABEL_CANCEL);
  showDialog(DIALOG_TYPE_INFO, checkCoolingFan, failToCheckCoolingFan, NULL);
}

//to preheat bed
void preheatBedToCheck(void)
{
  GUI_SetColor(BLACK); 
  GUI_SetBkColor(MAT_LOWWHITE);

  while(true)
  {
    preheatCurTime = OS_GetTimeMs();

    totalminTime = (preheatCurTime - preheatBeginTime) / (1000 * 60);        // m unit
    totalsecTime = ((preheatCurTime - preheatBeginTime) % (1000 * 60))/1000; // s unit 

    char tempstr[50];
    sprintf(tempstr, "%02d분:%02d초 경과", totalminTime, totalsecTime);

    GUI_DispString(230, 170, (uint8_t *)tempstr);

    sprintf(tempstr, "온도:%4d°C,", heatGetCurrentTemp(BED));
    
    GUI_DispString(70, 170, (uint8_t *)tempstr);    

    if(totalminTime>BED_PREHEAT_TIME)
    {
      char cannotHeatStr[50] = "베드 예열이 늦습니다.";
      popupReminder(DIALOG_TYPE_INFO, LABEL_WARNING, (uint8_t *)cannotHeatStr);
      break;
    }
    else if(heatGetCurrentTemp(BED) >= bedTestTemp)
    {
      bedResultTempMinTime = totalminTime;
      bedResultTempSecTime = totalsecTime;

      char finishedHeatTest[50] = "예열 테스트 완료.";

      char resultPreheat[100];

      sprintf(resultPreheat, "노즐 : %02d분:%02d초 \n베드 : %02d분:%02d초", nozzleResultTempMinTime, nozzleResultTempSecTime, bedResultTempMinTime, bedResultTempSecTime);

      setDialogText((uint8_t *)finishedHeatTest, (uint8_t *)resultPreheat, LABEL_CONFIRM, LABEL_BACKGROUND);
      showDialog(DIALOG_TYPE_INFO, NULL, NULL, NULL);       

      break;
    }

    loopProcess();
  }
}

//to preheat nozzle
void preheatNozzleToCheck(void)
{
  GUI_SetColor(BLACK); 
  GUI_SetBkColor(MAT_LOWWHITE);

  char noteCheckBedHeating [100]        = "베드 예열이 시작되었습니다.\n확인을 누르고 기다려주세요.";

  while (true)
  {
    preheatCurTime = OS_GetTimeMs();

    totalminTime = (preheatCurTime - preheatBeginTime) / (1000 * 60);        // m unit
    totalsecTime = ((preheatCurTime - preheatBeginTime) % (1000 * 60))/1000; // s unit    

    char tempstr[50];

    sprintf(tempstr, "%02d분:%02d초 경과", totalminTime, totalsecTime);
    GUI_DispString(230, 170, (uint8_t *)tempstr);

    sprintf(tempstr, "온도:%4d°C,", heatGetCurrentTemp(curExtruder_index));    
    GUI_DispString(70, 170, (uint8_t *)tempstr);    

    if(totalminTime>NOZZLE_PREHEAT_TIME)
    {
      char cannotHeatStr[50] = "노즐 예열이 늦습니다.";
      popupReminder(DIALOG_TYPE_INFO, LABEL_WARNING, (uint8_t *)cannotHeatStr);
      break;
    }
    else if(heatGetCurrentTemp(curExtruder_index) >= nozzleTestTemp)
    {
      nozzleResultTempMinTime = totalminTime;
      nozzleResultTempSecTime = totalsecTime;

      preheatBeginTime = OS_GetTimeMs(); 

      heatSetTargetTemp(BED,bedTestTemp);   

      setDialogText(LABEL_CHECK, (uint8_t *)noteCheckBedHeating, LABEL_CONFIRM, LABEL_BACKGROUND);
      showDialog(DIALOG_TYPE_INFO, preheatBedToCheck, NULL, NULL);

      break;
    }

    loopProcess();
  }  
}

//to check heat
void checkHeating(void)
{
  preheatBeginTime = OS_GetTimeMs();  

  char noteCheckNozzleHeating[100]        = "노즐 예열이 시작되었습니다.\n확인을 누르고 기다려주세요.";

  heatSetTargetTemp(curExtruder_index,nozzleTestTemp);    
  
  setDialogText(LABEL_CHECK, (uint8_t *)noteCheckNozzleHeating, LABEL_CONFIRM, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, preheatNozzleToCheck, NULL, NULL);

  GUI_SetColor(WHITE);
  GUI_SetBkColor(BLACK);
}

//repeat to change Fig.1 to Fig.2 and Fig.2 to Fig.1
void changeFigToCheckSpring(void)
{
  preheatCurTime = OS_GetTimeMs();

  if((preheatCurTime-preheatBeginTime)>2500)
  {
    ICON_ReadDisplay(300,100,ICON_CHECK_SPRING);
  }
  else
  {
    ICON_ReadDisplay(300,100,ICON_CHECK_SPRING_CLOSEUP);
  }

  if((preheatCurTime-preheatBeginTime)>5000)
  {
    preheatBeginTime = OS_GetTimeMs();
  }   
}

//display Fig
void filamentEndFig(void)
{
  ICON_ReadDisplay(300,100,ICON_CHECK_FILAMENT_END);  
}

//start filament test
void insertFilamentInExtruder(void)
{  
  startTesting();
  storeCmd("G92 E0\n");
  storeCmd("M415\n");
  infoMenu.menu[++infoMenu.cur] = menuLoadCheckFilament;
}

//check filament end
void checkFilamentEnd(void)
{
  char noteCheckFilamentEnd[100]   = "필라멘트 끝을\n뾰족하게 잘라\n주고, 익스트루더\n입구에 넣습니다.";  

  setDialogText(LABEL_CHECK, (uint8_t *)noteCheckFilamentEnd, LABEL_NEXT, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, insertFilamentInExtruder, NULL, filamentEndFig);
}

//Process to insert filament
void processInsertFilament(void)
{
  char noteCheckSpring[140]        = "익스트루더의\n스프링 길이가\n19~20mm 사이 인지\n확인하세요.";  

  preheatBeginTime = OS_GetTimeMs();  
  
  setDialogText(LABEL_CHECK, (uint8_t *)noteCheckSpring, LABEL_NEXT, LABEL_BACKGROUND);
  showDialog(DIALOG_TYPE_INFO, checkFilamentEnd, NULL, changeFigToCheckSpring);  
}


void menuReadyCheck(void)
{
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS readyCheckItems = {
    // title
    LABEL_READY_CHECK,
    // icon                         label
    {{ICON_FAN,                     LABEL_CHECK_FAN}, 
     {ICON_HEAT,                    LABEL_CHECK_HEAT},
     {ICON_CHECK_XYZHOME,           LABEL_CHECK_XYZHOME},     
     {ICON_LOAD,                    LABEL_USE_FILAMENT}, 
     {ICON_Z_CALIBRATION,           LABEL_ADJUST_X_LEVEL},
     {ICON_LIVE_LEVELING,           LABEL_CHECK_LEVELING},     
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK}}
  };  

  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&readyCheckItems);

  while (infoMenu.menu[infoMenu.cur] == menuReadyCheck)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
    case KEY_ICON_0:
      storeCmd("M102\n");
      checkMainFan();
      break;

    case KEY_ICON_1:
      checkHeating();
      break;

    case KEY_ICON_2:
      storeCmd("M417\n");
      break;

    case KEY_ICON_3:      
      if (heatGetCurrentTemp(curExtruder_index) < (nozzleTestTemp-5))
      { // low temperature warning
        char tempMsg[120] = "온도가 낮습니다.\n예열을 시작하겠습니다.\n잠시 후 다시 실행해주세요.";
        setDialogText(LABEL_WARNING, (uint8_t *)tempMsg, LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_ERROR, extrusionMinTemp_OK, NULL, NULL);
        infoMenu.cur -= 2;
      }
      else
      {
        processInsertFilament();
      }
      break;

    case KEY_ICON_4:
      startTesting();
      setTestingProgress(START_PROGRESS_NUM);
      storeCmd("M416\n");
      infoMenu.menu[++infoMenu.cur] = menuInclineLevelCali;
      break;

    case KEY_ICON_5:      
      if ((heatGetCurrentTemp(curExtruder_index) < (nozzleTestTemp-5))||(heatGetCurrentTemp(BED) < (bedTestTemp-5)))
      { // low temperature warning
        char tempMsg[120] = "온도가 낮습니다.\n예열을 시작하겠습니다.\n잠시 후 다시 실행해주세요.";
        setDialogText(LABEL_WARNING, (uint8_t *)tempMsg, LABEL_CONFIRM, LABEL_BACKGROUND);
        showDialog(DIALOG_TYPE_ERROR, extrusionMinTemp_OK, NULL, NULL);
        heatSetTargetTemp(BED,bedTestTemp);
        infoMenu.cur -= 2; 
      }
      else
      {
        startTesting();
        storeCmd("M418\n");
        infoMenu.menu[++infoMenu.cur] = menuZoffsetLiveSet;
      }
      break;    

    case KEY_ICON_6:      
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
