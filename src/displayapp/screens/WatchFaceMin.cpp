/*
 * This file is part of the Infinitime distribution (https://github.com/InfiniTimeOrg/Infinitime).
 * Copyright (c) 2021 Kieran Cawthray.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * WatchFaceMin watchface for Infinitime created by Kieran Cawthray
 * Based on WatchFaceDigital
 * Style/layout copied from TimeStyle for Pebble by Dan Tilden (github.com/tilden)
 */

#include "displayapp/screens/WatchFaceMin.h"
#include <date/date.h>
#include <lvgl/lvgl.h>
#include <cstdio>
#include <displayapp/Colors.h>
#include "displayapp/screens/BatteryIcon.h"
#include "displayapp/screens/BleIcon.h"
#include "displayapp/screens/NotificationIcon.h"
#include "displayapp/screens/Symbols.h"
#include "components/battery/BatteryController.h"
#include "components/ble/BleController.h"
#include "components/ble/NotificationManager.h"
#include "components/motion/MotionController.h"
#include "components/settings/Settings.h"
#include "displayapp/DisplayApp.h"

using namespace Pinetime::Applications::Screens;

//namespace {
  //void event_handler(lv_obj_t* obj, lv_event_t event) {
    //auto* screen = static_cast<WatchFaceMin*>(obj->user_data);
    //screen->UpdateSelected(obj, event);
  //}
//}

WatchFaceMin::WatchFaceMin(DisplayApp* app,
                             Controllers::DateTime& dateTimeController,
                             Controllers::Battery& batteryController,
                             Controllers::Ble& bleController,
                             Controllers::NotificationManager& notificatioManager,
                             Controllers::Settings& settingsController,
                             Controllers::MotionController& motionController)
  : Screen(app),
    currentDateTime {{}},
    dateTimeController {dateTimeController},
    batteryController {batteryController},
    bleController {bleController},
    notificatioManager {notificatioManager},
    settingsController {settingsController},
    motionController {motionController} {

  displayedChar[0] = 0;
  displayedChar[1] = 0;
  displayedChar[2] = 0;
  displayedChar[3] = 0;
  displayedChar[4] = 0;

  // Create a 200px wide background rectangle
  //timebar = lv_obj_create(lv_scr_act(), nullptr);
  //lv_obj_set_style_local_bg_color(timebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  //lv_obj_set_size(timebar, 240, 240);
  //lv_obj_align(timebar, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 0);

  // Display the time
  timeDD1 = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(timeDD1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &stm_80);
  lv_obj_set_style_local_text_color(timeDD1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_opa(timeDD1, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 75);
  lv_label_set_text(timeDD1, "0");
  lv_obj_align(timeDD1, lv_scr_act(), LV_ALIGN_CENTER, -75, 0);

  timeDD2 = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(timeDD2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &stm_80);
  lv_obj_set_style_local_text_color(timeDD2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_opa(timeDD2, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 175);
  lv_label_set_text(timeDD2, "0");
  lv_obj_align(timeDD2, lv_scr_act(), LV_ALIGN_CENTER, -30, 0);

  lv_obj_t* colon = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(colon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &stm_80);
  lv_obj_set_style_local_text_color(colon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_opa(colon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 175);
  lv_label_set_text(colon, ":");
  lv_obj_align(colon, lv_scr_act(), LV_ALIGN_CENTER, 0, -15);

  timeDD3 = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(timeDD3, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &stm_80);
  lv_obj_set_style_local_text_color(timeDD3, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_opa(timeDD3, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 75);
  lv_label_set_text(timeDD3, "0");
  lv_obj_align(timeDD3, lv_scr_act(), LV_ALIGN_CENTER,30, 0);

  timeDD4 = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_font(timeDD4, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, &stm_80);
  lv_obj_set_style_local_text_color(timeDD4, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_opa(timeDD4, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 175);
  lv_label_set_text(timeDD4, "0");
  lv_obj_align(timeDD4, lv_scr_act(), LV_ALIGN_CENTER, 75, 0);

  //datebar = lv_obj_create(lv_scr_act(), nullptr);
  //lv_obj_set_style_local_bg_color(datebar, LV_BTN_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
  //lv_obj_set_size(datebar, 140, 40);
  //lv_obj_align(datebar, lv_scr_act(), LV_ALIGN_IN_TOP_MID, 0, 0);

  //dateDayOfWeek = lv_label_create(lv_scr_act(), nullptr);
  //lv_obj_set_style_local_text_color(dateDayOfWeek, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  //lv_label_set_text(dateDayOfWeek, "THU");
  //lv_obj_align(dateDayOfWeek, datebar, LV_ALIGN_IN_LEFT_MID, 0, 0);

  //dateDay = lv_label_create(lv_scr_act(), nullptr);
  //lv_obj_set_style_local_text_color(dateDay, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  //lv_label_set_text(dateDay, "25");
  //lv_obj_align(dateDay, datebar, LV_ALIGN_CENTER, 0, 0);

  //dateMonth = lv_label_create(lv_scr_act(), nullptr);
  //lv_obj_set_style_local_text_color(dateMonth, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  //lv_label_set_text(dateMonth, "MAR");
  //lv_obj_align(dateMonth, datebar, LV_ALIGN_IN_RIGHT_MID, 0, 0);

  dateFull = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(dateFull, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_opa(dateFull, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 80);
  lv_label_set_text(dateFull, "FEB 02");
  lv_obj_align(dateFull, lv_scr_act(), LV_ALIGN_CENTER, 0, -90);

  batteryText = lv_label_create(lv_scr_act(), nullptr);
  lv_obj_set_style_local_text_color(batteryText, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
  lv_obj_set_style_local_text_opa(batteryText, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, 80);
  lv_label_set_text(batteryText, "80%");
  lv_obj_align(batteryText, lv_scr_act(), LV_ALIGN_CENTER, 0, 90);

  //batteryIcon = lv_label_create(lv_scr_act(), nullptr);
  //lv_label_set_text(batteryIcon, Symbols::batteryFull);
  //lv_obj_align(batteryIcon, lv_scr_act(), LV_ALIGN_CENTER, 0, 90);

  taskRefresh = lv_task_create(RefreshTaskCallback, LV_DISP_DEF_REFR_PERIOD, LV_TASK_PRIO_MID, this);
  Refresh();
}

WatchFaceMin::~WatchFaceMin() {
  lv_task_del(taskRefresh);
  lv_obj_clean(lv_scr_act());
}

bool WatchFaceMin::OnTouchEvent(Pinetime::Applications::TouchEvents event) {
  if ((event == Pinetime::Applications::TouchEvents::DoubleTap) && (lv_obj_get_hidden(btnRandom) == false)) {
    return true;
  }
  return false;
}


bool WatchFaceMin::OnButtonPushed() {
  return false;
}

void WatchFaceMin::Refresh() {


  batteryPercentRemaining = batteryController.PercentRemaining();
  if (batteryPercentRemaining.IsUpdated()) {
    auto batteryPercent = batteryPercentRemaining.Get();
    //if (batteryPercent == 100) {
      //lv_obj_set_style_local_text_color(batteryIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_GREEN);
    //} else {
      //lv_obj_set_style_local_text_color(batteryIcon, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);
    //}
    //lv_label_set_text(batteryIcon, BatteryIcon::GetBatteryIcon(batteryPercent));
    //char percentChar[3];
    //sprintf(batteryText, "%d%", static_cast<int>(batteryPercent));
    lv_label_set_text_fmt(batteryText, "%d%%", static_cast<int>(batteryPercent));
  }
  lv_obj_align(batteryText, lv_scr_act(), LV_ALIGN_CENTER, 0, 90);

  currentDateTime = dateTimeController.CurrentDateTime();
  if (currentDateTime.IsUpdated()) {
    auto newDateTime = currentDateTime.Get();

    auto dp = date::floor<date::days>(newDateTime);
    auto time = date::make_time(newDateTime - dp);
    auto yearMonthDay = date::year_month_day(dp);

    auto year = static_cast<int>(yearMonthDay.year());
    auto month = static_cast<Pinetime::Controllers::DateTime::Months>(static_cast<unsigned>(yearMonthDay.month()));
    auto day = static_cast<unsigned>(yearMonthDay.day());
    auto dayOfWeek = static_cast<Pinetime::Controllers::DateTime::Days>(date::weekday(yearMonthDay).iso_encoding());

    int hour = time.hours().count();
    auto minute = time.minutes().count();

    char minutesChar[3];
    sprintf(minutesChar, "%02d", static_cast<int>(minute));

    char hoursChar[3];
    char ampmChar[5];
    if (settingsController.GetClockType() == Controllers::Settings::ClockType::H24) {
      sprintf(hoursChar, "%02d", hour);
    } else {
      if (hour == 0 && hour != 12) {
        hour = 12;
      } else if (hour == 12 && hour != 0) {
        hour = 12;
      } else if (hour > 12 && hour != 0) {
        hour = hour - 12;
      }
      sprintf(hoursChar, "%02d", hour);
    }

    char d1Char[1];
    char d2Char[1];
    char d3Char[1];
    char d4Char[1];

    if (hoursChar[0] != displayedChar[0] or hoursChar[1] != displayedChar[1] or minutesChar[0] != displayedChar[2] or
        minutesChar[1] != displayedChar[3]) {
      displayedChar[0] = hoursChar[0];
      displayedChar[1] = hoursChar[1];
      displayedChar[2] = minutesChar[0];
      displayedChar[3] = minutesChar[1];

      d1Char[0] = displayedChar[0];
      d2Char[0] = displayedChar[1];
      d3Char[0] = displayedChar[2];
      d4Char[0] = displayedChar[3];

      //lv_label_set_text_fmt(timeDD1, "%s", hoursChar[0]);
      lv_label_set_text_fmt(timeDD1, "%s", d1Char);
      lv_label_set_text_fmt(timeDD2, "%s", d2Char);
      lv_label_set_text_fmt(timeDD3, "%s", d3Char);
      lv_label_set_text_fmt(timeDD4, "%s", d4Char);

      //lv_label_set_text_fmt(timeDD1, "0");
      //lv_label_set_text_fmt(timeDD2, "2");
      //lv_label_set_text_fmt(timeDD3, "0");
      //lv_label_set_text_fmt(timeDD4, "6");

    }

    if ((year != currentYear) || (month != currentMonth) || (dayOfWeek != currentDayOfWeek) || (day != currentDay)) {
      lv_label_set_text_fmt(dateFull, "%s %d, %d", dateTimeController.MonthShortToString(), day, year);
      lv_obj_realign(dateFull);
      //lv_label_set_text_fmt(dateDayOfWeek, "%s", dateTimeController.DayOfWeekShortToString());
      //lv_label_set_text_fmt(dateDay, "%d", day);
      //lv_label_set_text_fmt(dateMonth, "%s", dateTimeController.MonthShortToString());

      currentYear = year;
      currentMonth = month;
      currentDayOfWeek = dayOfWeek;
      currentDay = day;
    }
  }

}

