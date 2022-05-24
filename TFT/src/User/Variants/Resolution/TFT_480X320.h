#ifndef _TFT_480_320_H_
#define _TFT_480_320_H_

#define LCD_WIDTH   480
#define LCD_HEIGHT  320

#ifndef BYTE_HEIGHT
  #define BYTE_HEIGHT 24
#endif
#ifndef BYTE_WIDTH
  #define BYTE_WIDTH  (BYTE_HEIGHT/2)
#endif

#ifndef LARGE_BYTE_HEIGHT
  #define LARGE_BYTE_HEIGHT 32
#endif
#ifndef LARGE_BYTE_WIDTH
  #define LARGE_BYTE_WIDTH  18
#endif

#define ICON_WIDTH   95
#define ICON_HEIGHT  95
#define TITLE_END_Y  40
#define ICON_START_Y (TITLE_END_Y+10)

#define LIST_ICON_WIDTH   85
#define LIST_ICON_HEIGHT  70

#define INFOBOX_WIDTH     215
#define INFOBOX_HEIGHT    95

#define SMALLICON_WIDTH   65
#define SMALLICON_HEIGHT  55

// Status screen menu
#define SSICON_COUNT              4
#define SSICON_WIDTH              ICON_WIDTH
#define SSICON_HEIGHT             ICON_HEIGHT
#define SSICON_VAL_Y0             75
#define SSICON_VAL_COLOR          BLACK
#define SSICON_NAME_Y0            7
#define SSICON_NAME_COLOR         WHITE
#define STATUS_MSG_ICON_XOFFSET   5
#define STATUS_MSG_ICON_YOFFSET   4 //5
#define STATUS_MSG_TITLE_XOFFSET  5
#define STATUS_MSG_BODY_XOFFSET   0
#define STATUS_MSG_BODY_YOFFSET   59
#define STATUS_MSG_BODY_BOTTOM    6
#define STATUS_GANTRY_YOFFSET     6

// cremaker status screen
#define SS_ICONS_X         20
#define SS_ICONS_Y         40
#define SS_MAINLOGO_X      170
#define SS_MAINLOGO_Y      40
#define SS_XYZVALUE_X      410 //test
#define SS_XYZVALUE_Y      57  //test
#define SS_XYZICON_X       424
#define SS_XYZICON_Y       57
#define SS_MAINMENU_X      25
#define SS_MAINMENU_Y      228
#define SS_INFOBOX_X       110
#define SS_INFOBOX_Y       228
#define SS_PRINT_X         388
#define SS_PRINT_Y         228

#define ICONS_W            30
#define ICONS_H            ICONS_W
#define ICONS_PIXEL_GAP    12
#define XYZ_ICON_W         32
#define XYZ_ICON_H         XYZ_ICON_W
#define XYZ_ICON_PIXEL_GAP 6
#define MAINMENE_W         67
#define MAINMENE_H         67
#define INFOBOX_W          260
#define INFOBOX_H          67
#define PRINT_W            67
#define PRINT_H            67

#define TOAST_HEIGHT       37

// cremaker printing screen

#define PS_ICONS_X           226
#define PS_ICONS_Y           72
#define PS_ICON_LAYER_X      PS_ICONS_X
#define PS_ICON_LAYER_Y      (PS_ICONS_Y + 170)

#define PS_PAUSE_X           388
#define PS_PAUSE_Y           50
#define PS_MORE_X            PS_PAUSE_X
#define PS_MORE_Y            (PS_PAUSE_Y + PS_BTN_GAP_H)
#define PS_STOP_PRINTING_X   PS_PAUSE_X
#define PS_STOP_PRINTING_Y   (PS_MORE_Y + PS_BTN_GAP_H)

#define PS_MAINLOGO_X        25
#define PS_MAINLOGO_Y        69
#define PS_TIMER_X           32
#define PS_TIMER_Y           255

#define PS_BTN_GAP_H         86
#define PS_MAINLOGO_H        170
#define PS_MAINLOGO_W        170

#define PS_MAINLOGO_CENTER_X ((PS_MAINLOGO_W/2)+PS_MAINLOGO_X)
#define PS_MAINLOGO_CENTER_Y ((PS_MAINLOGO_H/2)+PS_MAINLOGO_Y)
#define PS_PROGRESS_NUM100_X (PS_MAINLOGO_CENTER_X - PS_PROGRESS_NUM_W - (PS_PROGRESS_NUM_W/2))
#define PS_PROGRESS_NUM100_Y (PS_MAINLOGO_CENTER_Y - (PS_PROGRESS_NUM_H/2))
#define PS_PROGRESS_NUM10_X  (PS_MAINLOGO_CENTER_X - PS_PROGRESS_NUM_W)
#define PS_PROGRESS_NUM10_Y  (PS_MAINLOGO_CENTER_Y - (PS_PROGRESS_NUM_H/2))
#define PS_PROGRESS_NUM1_X   (PS_MAINLOGO_CENTER_X - (PS_PROGRESS_NUM_W/2))
#define PS_PROGRESS_NUM1_Y   (PS_MAINLOGO_CENTER_Y - (PS_PROGRESS_NUM_H/2))

#define PS_PAUSE_W           67
#define PS_PAUSE_H           PS_PAUSE_W
#define PS_MORE_W            67
#define PS_MORE_H            PS_MORE_W
#define PS_STOP_PRINTING_W   67
#define PS_STOP_PRINTING_H   PS_STOP_PRINTING_W
#define PS_ICON_W            36
#define PS_ICON_H            36
#define PS_ICON_LAYER_W      PS_ICON_W
#define PS_ICON_LAYER_H      38
#define PS_TIMER_W           36
#define PS_STATE_VAL_W       90
#define PS_STATE_VAL_H       BYTE_HEIGHT
#define PS_TIME_VAL_W        100
#define PS_TIME_VAL_H        BYTE_HEIGHT

#define PS_VALUE_GAP_X       8
#define PS_VALUE_GAP_Y       6

#define PS_PROGRESS_NUM_W    37
#define PS_PROGRESS_NUM_H    61

#define INFOBOX_RADIUS_PIXEL 20

#define NAME_LARGE_FONT           false
#define VAL_LARGE_FONT            false

//Heating Menu
#define PREHEAT_TITLE_Y   4
#define PREHEAT_TOOL_Y    43
#define PREHEAT_BED_Y     74

// Printing menu
#define PICON_LG_WIDTH     154
#define PICON_SM_WIDTH     121
#define PICON_HEIGHT       50
#define PICON_SPACE_X      ((LCD_WIDTH - PICON_LG_WIDTH*2 - PICON_SM_WIDTH - SPACE_X )/2)
#define PICON_SPACE_Y      (ICON_HEIGHT + SPACE_Y - PICON_HEIGHT*2)/2
#define PICON_TITLE_X      52
#define PICON_TITLE_Y      1
#define PICON_VAL_X        PICON_TITLE_X
#define PICON_VAL_Y        (PICON_HEIGHT/2)
#define PICON_VAL_LG_EX    150
#define PICON_VAL_SM_EX    117

// Popup menu
#define POPUP_RECT_WINDOW         {SPACE_X*2, TITLE_END_Y, LCD_WIDTH - SPACE_X*2, LCD_HEIGHT - SPACE_Y}
#define POPUP_TITLE_HEIGHT        50
#define POPUP_BOTTOM_HEIGHT       70
#define POPUP_BUTTON_HEIGHT       50
#define POPUP_RECT_SINGLE_CONFIRM {175, LCD_HEIGHT- SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, 305, LCD_HEIGHT - SPACE_Y - 10}
#define POPUP_RECT_DOUBLE_CONFIRM {85,  LCD_HEIGHT- SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, 225, LCD_HEIGHT - SPACE_Y - 10}
#define POPUP_RECT_DOUBLE_CANCEL  {255, LCD_HEIGHT- SPACE_Y - 10 - POPUP_BUTTON_HEIGHT, 395, LCD_HEIGHT - SPACE_Y - 10}

// The offset of the model preview icon in the gcode file
#define MODEL_PREVIEW_OFFSET 0xC529

#endif
