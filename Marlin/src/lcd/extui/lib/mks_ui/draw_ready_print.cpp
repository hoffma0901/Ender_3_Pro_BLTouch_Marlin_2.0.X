/**
 * Marlin 3D Printer Firmware
 * Copyright (c) 2020 MarlinFirmware [https://github.com/MarlinFirmware/Marlin]
 *
 * Based on Sprinter and grbl.
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 */
#include "../../../../inc/MarlinConfigPre.h"

#if HAS_TFT_LVGL_UI

#include "draw_ready_print.h"
#include "draw_tool.h"
#include <lv_conf.h>
#include "tft_lvgl_configuration.h"
#include "mks_hardware_test.h"
#include "draw_ui.h"

#include <lvgl.h>

#include "../../../../module/temperature.h"
#include "../../../../inc/MarlinConfig.h"

#if ENABLED(TOUCH_SCREEN_CALIBRATION)
  #include "../../../tft_io/touch_calibration.h"
  #include "draw_touch_calibration.h"
#endif

#include <stdio.h>

#define ICON_POS_Y          38
#define TARGET_LABEL_MOD_Y -36
#define LABEL_MOD_Y         30
#define SECOND_EXT_MOD_Y   100

extern lv_group_t*  g;
static lv_obj_t *scr;
static lv_obj_t *labelExt1, *labelExt1Target, *labelFan;

#if HAS_MULTI_EXTRUDER
  static lv_obj_t *labelExt2, *labelExt2Target;
#endif

#if HAS_HEATED_BED
  static lv_obj_t *labelBed, *labelBedTarget;
#endif

#if ENABLED(MKS_TEST)
  uint8_t curent_disp_ui = 0;
#endif

enum { ID_TOOL = 1, ID_SET, ID_PRINT };

static void event_handler(lv_obj_t *obj, lv_event_t event) {
  if (event != LV_EVENT_RELEASED) return;
  lv_clear_ready_print();
  switch (obj->mks_obj_id) {
    case ID_TOOL:   lv_draw_tool(); break;
    case ID_SET:    lv_draw_set(); break;
    case ID_PRINT:  lv_draw_print_file(); break;
  }
}

lv_obj_t *limit_info, *det_info;
lv_style_t limit_style, det_style;
void disp_Limit_ok() {
  limit_style.text.color.full = 0xFFFF;
  lv_obj_set_style(limit_info, &limit_style);
  lv_label_set_text(limit_info, "Limit:ok");
}
void disp_Limit_error() {
  limit_style.text.color.full = 0xF800;
  lv_obj_set_style(limit_info, &limit_style);
  lv_label_set_text(limit_info, "Limit:error");
}

void disp_det_ok() {
  det_style.text.color.full = 0xFFFF;
  lv_obj_set_style(det_info, &det_style);
  lv_label_set_text(det_info, "det:ok");
}
void disp_det_error() {
  det_style.text.color.full = 0xF800;
  lv_obj_set_style(det_info, &det_style);
  lv_label_set_text(det_info, "det:error");
}

lv_obj_t *e1, *e2, *e3, *bed;
void mks_disp_test() {
  char buf[30] = {0};
  sprintf_P(buf, PSTR("e1:%d"), thermalManager.degHotend(0));
  lv_label_set_text(e1, buf);
  #if HAS_MULTI_HOTEND
    sprintf_P(buf, PSTR("e2:%d"), thermalManager.degHotend(1));
    lv_label_set_text(e2, buf);
  #endif
  #if HAS_HEATED_BED
    sprintf_P(buf, PSTR("bed:%d"), thermalManager.degBed());
    lv_label_set_text(bed, buf);
  #endif
}

void lv_draw_ready_print() {
  char buf[30] = {0};
  lv_obj_t *buttonTool;

  disp_state_stack._disp_index = 0;
  ZERO(disp_state_stack._disp_state);
  scr = lv_screen_create(PRINT_READY_UI, "");

  if (mks_test_flag == 0x1E) {
    // Create image buttons
    buttonTool = lv_imgbtn_create(scr, "F:/bmp_tool.bin", event_handler, ID_TOOL);

    lv_obj_set_pos(buttonTool, 360, 180);

    lv_obj_t *label_tool = lv_label_create_empty(buttonTool);
    if (gCfgItems.multiple_language) {
      lv_label_set_text(label_tool, main_menu.tool);
      lv_obj_align(label_tool, buttonTool, LV_ALIGN_IN_BOTTOM_MID, 0, BUTTON_TEXT_Y_OFFSET);
    }

    e1 = lv_label_create_empty(scr);
    lv_obj_set_pos(e1, 20, 20);
    sprintf_P(buf, PSTR("e1:  %d"), thermalManager.degHotend(0));
    lv_label_set_text(e1, buf);

    #if HAS_MULTI_HOTEND
      e2 = lv_label_create_empty(scr);
      lv_obj_set_pos(e2, 20, 45);
      sprintf_P(buf, PSTR("e1:  %d"), thermalManager.degHotend(1));
      lv_label_set_text(e2, buf);
    #endif

    #if HAS_HEATED_BED
      bed = lv_label_create_empty(scr);
      lv_obj_set_pos(bed, 20, 95);
      sprintf_P(buf, PSTR("bed:  %d"), thermalManager.degBed());
      lv_label_set_text(bed, buf);
    #endif

    limit_info = lv_label_create_empty(scr);

    lv_style_copy(&limit_style, &lv_style_scr);
    limit_style.body.main_color.full = 0x0000;
    limit_style.body.grad_color.full = 0x0000;
    limit_style.text.color.full      = 0xFFFF;
    lv_obj_set_style(limit_info, &limit_style);

    lv_obj_set_pos(limit_info, 20, 120);
    lv_label_set_text(limit_info, " ");

    det_info = lv_label_create_empty(scr);

    lv_style_copy(&det_style, &lv_style_scr);
    det_style.body.main_color.full = 0x0000;
    det_style.body.grad_color.full = 0x0000;
    det_style.text.color.full      = 0xFFFF;
    lv_obj_set_style(det_info, &det_style);

    lv_obj_set_pos(det_info, 20, 145);
    lv_label_set_text(det_info, " ");
  }
  else {
    lv_big_button_create(scr, "F:/bmp_tool.bin", main_menu.tool, 20, 180, event_handler, ID_TOOL);
    lv_big_button_create(scr, "F:/bmp_set.bin", main_menu.set, 180, 180, event_handler, ID_SET);
    lv_big_button_create(scr, "F:/bmp_printing.bin", main_menu.print, 340, 180, event_handler, ID_PRINT);

    // Monitoring
    lv_obj_t *buttonExt1 = lv_img_create(scr, NULL);
    #if HAS_MULTI_EXTRUDER
      lv_obj_t *buttonExt2 = lv_img_create(scr, NULL);
    #endif
    #if HAS_HEATED_BED
      lv_obj_t *buttonBedstate = lv_img_create(scr, NULL);
    #endif
    lv_obj_t *buttonFanstate = lv_img_create(scr, NULL);

    lv_img_set_src(buttonExt1, "F:/bmp_ext1_state.bin");
    #if HAS_MULTI_EXTRUDER
      lv_img_set_src(buttonExt2, "F:/bmp_ext2_state.bin");
    #endif
    #if HAS_HEATED_BED
      lv_img_set_src(buttonBedstate, "F:/bmp_bed_state.bin");
    #endif
    lv_img_set_src(buttonFanstate, "F:/bmp_fan_state.bin");

    lv_obj_set_pos(buttonExt1, 55, ICON_POS_Y);
    #if HAS_MULTI_EXTRUDER
      lv_obj_set_pos(buttonExt2, 55, ICON_POS_Y + SECOND_EXT_MOD_Y);
    #endif
    #if HAS_HEATED_BED
      lv_obj_set_pos(buttonBedstate, 210, ICON_POS_Y);
    #endif
    lv_obj_set_pos(buttonFanstate, 380, ICON_POS_Y);

    labelExt1 = lv_label_create(scr, 55, LABEL_MOD_Y, nullptr);
    labelExt1Target = lv_label_create(scr, 55, LABEL_MOD_Y, nullptr);

    #if HAS_MULTI_EXTRUDER
      labelExt2 = lv_label_create(scr, 55, LABEL_MOD_Y + SECOND_EXT_MOD_Y, nullptr);
      labelExt2Target = lv_label_create(scr, 55, LABEL_MOD_Y + SECOND_EXT_MOD_Y, nullptr);
    #endif

    #if HAS_HEATED_BED
      labelBed = lv_label_create(scr, 210, LABEL_MOD_Y, nullptr);
      labelBedTarget = lv_label_create(scr, 210, LABEL_MOD_Y, nullptr);
    #endif

    labelFan = lv_label_create(scr, 380, 80, nullptr);

    sprintf_P(buf, PSTR("%d"), thermalManager.degHotend(0));
    lv_label_set_text(labelExt1, buf);
    lv_obj_align(labelExt1, buttonExt1, LV_ALIGN_CENTER, 0, LABEL_MOD_Y);
    sprintf_P(buf, PSTR("-> %d"), thermalManager.degTargetHotend(0));
    lv_label_set_text(labelExt1Target, buf);
    lv_obj_align(labelExt1Target, buttonExt1, LV_ALIGN_CENTER, 0, TARGET_LABEL_MOD_Y);

    #if HAS_MULTI_EXTRUDER
      sprintf_P(buf, PSTR("%d"), thermalManager.degHotend(1));
      lv_label_set_text(labelExt2, buf);
      lv_obj_align(labelExt2, buttonExt2, LV_ALIGN_CENTER, 0, LABEL_MOD_Y);
      sprintf_P(buf, PSTR("-> %d"), thermalManager.degTargetHotend(1));
      lv_label_set_text(labelExt2Target, buf);
      lv_obj_align(labelExt2Target, buttonExt2, LV_ALIGN_CENTER, 0, TARGET_LABEL_MOD_Y);
    #endif

    #if HAS_HEATED_BED
      sprintf_P(buf, PSTR("%d"), thermalManager.degBed());
      lv_label_set_text(labelBed, buf);
      lv_obj_align(labelBed, buttonBedstate, LV_ALIGN_CENTER, 0, LABEL_MOD_Y);
      sprintf_P(buf, PSTR("-> %d"), thermalManager.degTargetBed());
      lv_label_set_text(labelBedTarget, buf);
      lv_obj_align(labelBedTarget, buttonBedstate, LV_ALIGN_CENTER, 0, TARGET_LABEL_MOD_Y);
    #endif

    sprintf_P(buf, PSTR("%d%%"), thermalManager.fanPercent(thermalManager.fan_speed[0]));
    lv_label_set_text(labelFan, buf);
    lv_obj_align(labelFan, buttonFanstate, LV_ALIGN_CENTER, 0, LABEL_MOD_Y);
  }

  #if ENABLED(TOUCH_SCREEN_CALIBRATION)
    // If calibration is required, let's trigger it now, handles the case when there is default value in configuration files
    if (!touch_calibration.calibration_loaded()) {
      lv_clear_ready_print();
      lv_draw_touch_calibration_screen();
    }
  #endif
}

void lv_clear_ready_print() {
  #if HAS_ROTARY_ENCODER
    if (gCfgItems.encoder_enable) lv_group_remove_all_objs(g);
  #endif
  lv_obj_del(scr);
}

#endif // HAS_TFT_LVGL_UI
