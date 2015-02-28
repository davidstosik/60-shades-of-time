#include <pebble.h>
#include "color_names.h"

#define DEBUG false

static Window *window;
static TextLayer *name_layer;
static TextLayer *time_layer;
static TextLayer *date_layer;
static BitmapLayer *separator_layer;
static char time_str[] = "00:00";
static char date_str[] = "Xxxxxxxxxxxxxxxxxxxxxxx 00";

static GColor8 get_color_from_minute(uint8_t minute) {
  uint8_t argb = GColorBlackARGB8 + minute + 1;

  if (argb >= GColorDarkGrayARGB8) { argb++; }
  if (argb >= GColorLightGrayARGB8) { argb++; }

  return (GColor8){ .argb = argb };
}

static GColor8 gcolor_fgcolor(GColor8 bgcolor) {
  // Inspiration: http://stackoverflow.com/a/946734/2341409
  uint8_t r, g, b;
  r = (bgcolor.argb & 0b110000) >> 4;
  g = (bgcolor.argb & 0b001100) >> 2;
  b = bgcolor.argb & 0b000011;
  float brightness = r*0.299 + g*0.587 + b*0.114;
  return brightness < 1.75 ? GColorWhite : GColorBlack;
}

void update_screen(struct tm *tick_time) {
  char *time_format = clock_is_24h_style() ? "%R" : "%I:%M";

  strftime(time_str, sizeof(time_str), time_format, tick_time);
  strftime(date_str, sizeof(date_str), "%B %e", tick_time);

  uint8_t color_index = DEBUG ? tick_time->tm_sec : tick_time->tm_min;

  text_layer_set_text(name_layer, color_names[color_index]);
  text_layer_set_text(time_layer, time_str);
  text_layer_set_text(date_layer, date_str);

  GColor8 bg = get_color_from_minute(color_index);
  window_set_background_color(window, bg);

  GColor fg_color = gcolor_fgcolor(bg);
  text_layer_set_text_color(name_layer, fg_color);
  text_layer_set_text_color(time_layer, fg_color);
  text_layer_set_text_color(date_layer, fg_color);
  bitmap_layer_set_background_color(separator_layer, fg_color);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  GFont font_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  GFont font_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  GFont font_roboto_49 = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);

  name_layer = text_layer_create((GRect) {
    .origin = { 0, 0 },
    .size = { bounds.size.w, 23 }
  });
  text_layer_set_text_alignment(name_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(name_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(name_layer, font_gothic_18);
  text_layer_set_background_color(name_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(name_layer));

  time_layer = text_layer_create((GRect) {
    .origin = { 0, bounds.size.h/2 + 10 },
    .size = { bounds.size.w - 10, 50 }
  });
  text_layer_set_text_alignment(time_layer, GTextAlignmentRight);
  text_layer_set_font(time_layer, font_roboto_49);
  text_layer_set_background_color(time_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  date_layer = text_layer_create((GRect) {
    .origin = { 10, bounds.size.h/2 - 19 },
    .size = { bounds.size.w - 20, 31 }
  });
  text_layer_set_text_alignment(date_layer, GTextAlignmentLeft);
  text_layer_set_font(date_layer, font_gothic_24_bold);
  text_layer_set_background_color(date_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(date_layer));

  separator_layer = bitmap_layer_create((GRect) {
    .origin = { 10, bounds.size.h/2 + 12 },
    .size = { bounds.size.w - 20, 2 }
  });
  layer_add_child(window_layer, bitmap_layer_get_layer(separator_layer));
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_screen(tick_time);
  if (units_changed & HOUR_UNIT) {// && settings->hour_vibes) {
    vibes_double_pulse();
  }
}

static void window_unload(Window *window) {
  text_layer_destroy(name_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  bitmap_layer_destroy(separator_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  TimeUnits update_freq = DEBUG ? SECOND_UNIT : MINUTE_UNIT;
  tick_timer_service_subscribe(update_freq, tick_handler);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
