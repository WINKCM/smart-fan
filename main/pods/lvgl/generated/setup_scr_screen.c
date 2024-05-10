/*
* Copyright 2024 NXP
* NXP Confidential and Proprietary. This software is owned or controlled by NXP and may only be used strictly in
* accordance with the applicable license terms. By expressly accepting such terms or by downloading, installing,
* activating and/or otherwise using the software, you are agreeing that you have read, and that you agree to
* comply with and are bound by, such license terms.  If you do not agree to be bound by the applicable license
* terms, then you may not retain, install, activate or otherwise use the software.
*/

#include "lvgl.h"
#include <stdio.h>
#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"


void setup_scr_screen(lv_ui *ui)
{
	//Write codes screen
	ui->screen = lv_obj_create(NULL);
	lv_obj_set_size(ui->screen, 160, 100);

	//Write style for screen, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_label_1
	ui->screen_label_1 = lv_label_create(ui->screen);
	lv_label_set_text(ui->screen_label_1, "100");
	lv_label_set_long_mode(ui->screen_label_1, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_label_1, 103, 33);
	lv_obj_set_size(ui->screen_label_1, 44, 28);

	//Write style for screen_label_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_label_1, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_opa(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_color(ui->screen_label_1, lv_color_hex(0xffffff), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_border_side(ui->screen_label_1, LV_BORDER_SIDE_FULL, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_label_1, 5, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_label_1, lv_color_hex(0x3ebed3), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_label_1, &lv_font_arial_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_label_1, 1, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_label_1, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_label_1, 8, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_label_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write codes screen_bar_1
	ui->screen_bar_1 = lv_bar_create(ui->screen);
	lv_obj_set_style_anim_time(ui->screen_bar_1, 1000, 0);
	lv_bar_set_mode(ui->screen_bar_1, LV_BAR_MODE_NORMAL);
	lv_bar_set_value(ui->screen_bar_1, 50, LV_ANIM_OFF);
	lv_obj_set_pos(ui->screen_bar_1, 10, 15);
	lv_obj_set_size(ui->screen_bar_1, 140, 10);

	//Write style for screen_bar_1, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_bar_1, 61, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_bar_1, lv_color_hex(0x3ebed3), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_bar_1, 10, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_bar_1, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Write style for screen_bar_1, Part: LV_PART_INDICATOR, State: LV_STATE_DEFAULT.
	lv_obj_set_style_bg_opa(ui->screen_bar_1, 255, LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_color(ui->screen_bar_1, lv_color_hex(0x3ebed3), LV_PART_INDICATOR|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_bar_1, 10, LV_PART_INDICATOR|LV_STATE_DEFAULT);

	//Write codes screen_label_2
	ui->screen_label_2 = lv_label_create(ui->screen);
	lv_label_set_text(ui->screen_label_2, "15:02");
	lv_label_set_long_mode(ui->screen_label_2, LV_LABEL_LONG_WRAP);
	lv_obj_set_pos(ui->screen_label_2, 15, 43);
	lv_obj_set_size(ui->screen_label_2, 57, 20);

	//Write style for screen_label_2, Part: LV_PART_MAIN, State: LV_STATE_DEFAULT.
	lv_obj_set_style_border_width(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_radius(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(ui->screen_label_2, lv_color_hex(0x3ebed3), LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_font(ui->screen_label_2, &lv_font_montserratMedium_16, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(ui->screen_label_2, 2, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_line_space(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_text_align(ui->screen_label_2, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_bg_opa(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_top(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_right(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_bottom(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_pad_left(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);
	lv_obj_set_style_shadow_width(ui->screen_label_2, 0, LV_PART_MAIN|LV_STATE_DEFAULT);

	//Update current screen layout.
	lv_obj_update_layout(ui->screen);

	
}
