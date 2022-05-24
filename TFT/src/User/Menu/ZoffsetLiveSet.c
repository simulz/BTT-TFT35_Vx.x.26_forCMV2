#include "ZoffsetLiveSet.h"
#include "includes.h"

TESTING infoTesting;

void babyReDrawZOLS(float z_offset, bool force_z_offset, bool skip_header)
{
  if (!skip_header)
  {
    if (infoMachineSettings.zProbe == ENABLED)
        GUI_DispString(SPACE_X, exhibitRect.y0, LABEL_PROBE_OFFSET);
    else if (infoMachineSettings.leveling == BL_MBL)
      GUI_DispString(exhibitRect.x0-4, exhibitRect.y0 + BYTE_HEIGHT + LARGE_BYTE_HEIGHT, LABEL_MBL);
    else
      GUI_DispString(exhibitRect.x0-4, exhibitRect.y0 + BYTE_HEIGHT + LARGE_BYTE_HEIGHT, LABEL_HOME_OFFSET);
  }

  char tempstr[20];

  GUI_POINT point_of = {exhibitRect.x0+180, exhibitRect.y0 }; //cremaker;
  
  setLargeFont(true);

  sprintf(tempstr, "% 6.2f", z_offset);

  if (force_z_offset)
    GUI_SetColor(infoSettings.reminder_color);
  else
    GUI_SetColor(infoSettings.font_color);

  GUI_DispStringRight(point_of.x, point_of.y, (uint8_t*) tempstr);

  // restore default font color
  GUI_SetColor(infoSettings.font_color);

  setLargeFont(false);
}

// draw text during test progress
void drawTextInDisplay(char *text, uint16_t lineNum)
{    
  char tempstr[50];
  uint16_t y;

  switch(lineNum)
  {
    case FRIST_LINE_TO_DISPALY:  y = FRIST_LINE_Y_VALUE;  break;
    case SECOND_LINE_TO_DISPALY: y = SECONE_LINE_Y_VALUE; break;
    case THIRD_LINE_TO_DISPALY:  y = THIRD_LINE_Y_VALUE;  break;
    case FOURTH_LINE_TO_DISPALY: y = FOURTH_LINE_Y_VALUE; break;
    case FTFTH_LINE_TO_DISPALY:  y = FTFTH_LINE_Y_VALUE;  break;
    case SIXTH_LINE_TO_DISPALY:  y = SXITH_LINE_Y_VALUE;  break;
    default: break;
  }

  sprintf(tempstr, text);
  GUI_DispString(TEXT_LINE_X_VALUE, y, (uint8_t *)tempstr);
}

void menuZoffsetLiveSet(void)
{  
  // 1 title, ITEM_PER_PAGE items (icon + label)
  MENUITEMS ZoffsetLiveSetItems = {
    // title
    LABEL_BABYSTEP, //나중에 바꿀것 제목
    // icon                         label
    {{ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACKGROUND,              LABEL_BACKGROUND},
     {ICON_BACK,                    LABEL_BACK},}
  };

  KEY_VALUES key_num = KEY_IDLE;
  float now_babystep, babystep, orig_babystep;
  float now_z_offset, z_offset, orig_z_offset, new_z_offset;
  bool force_z_offset;
  float (* offsetGetValue)(void);   // get current Z offset
  float (*offsetSetValue)(float);   // set current Z offset

  offsetGetValue = probeOffsetGetValue;
  offsetSetValue = probeOffsetSetValue;

  now_babystep = babystep = orig_babystep = babystepGetValue();
  now_z_offset = z_offset = orig_z_offset = new_z_offset = offsetGetValue();
  force_z_offset = false;

  if (infoMachineSettings.EEPROM == 1)
  {
    ZoffsetLiveSetItems.items[KEY_ICON_3].icon = ICON_EEPROM_SAVE;
    ZoffsetLiveSetItems.items[KEY_ICON_3].label.index = LABEL_SAVE;
  }

  menuDrawPage(&ZoffsetLiveSetItems);
  babyReDrawZOLS(now_z_offset, force_z_offset, false);

  if(isPrinting())
  {
    drawTextInDisplay("바닥 출력 진행중에", FRIST_LINE_TO_DISPALY);
  }
  else
  {
    drawTextInDisplay("출력을 진행합니다.", FRIST_LINE_TO_DISPALY);
  }
  
  drawTextInDisplay("라인 상태를 확인하고,", SECOND_LINE_TO_DISPALY);
  drawTextInDisplay("조그 버튼을 돌려 높이", THIRD_LINE_TO_DISPALY);
  drawTextInDisplay("를 조절합니다.       ", FOURTH_LINE_TO_DISPALY);
  drawTextInDisplay("완료 후 저장합니다.  ", FTFTH_LINE_TO_DISPALY);

  while (infoMenu.menu[infoMenu.cur] == menuZoffsetLiveSet)
  { 
    babystep = babystepGetValue();  // always load current babystep

    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_3:
        if (infoMachineSettings.EEPROM == 1)
        {
          offsetSetValue(z_offset);  // set new Z offset. Required if current Z offset is not changed applying babystep changes (e.g. no BABYSTEP_ZPROBE_OFFSET is set in Marlin FW)

          setDialogText(ZoffsetLiveSetItems.title.index, LABEL_SAVE_VALUE_AFTER_TEST, LABEL_YES, LABEL_NO);
          showDialog(DIALOG_TYPE_QUESTION, saveEepromSettings, NULL, NULL);
        }
        finishTesting();

        if(isPrinting())
        {
          storeCmd("M419 F\n");
        }
        else
        {
          storeCmd("M419\n");
        }        

        infoMenu.cur--;
        break;
      
      case KEY_ICON_7:
        finishTesting();

        if(isPrinting())
        {
          storeCmd("M419 F\n");
        }
        else
        {
          storeCmd("M419\n");
        } 

        infoMenu.cur--;
        break;

      default:
        break;
    }

    z_offset = offsetGetValue();
    babyReDrawZOLS(z_offset, force_z_offset, false);
    loopProcess();
  }
}
