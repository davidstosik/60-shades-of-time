#include <pebble.h>
#include "colorize.h"

#define DEBUG false

static Window *window;
static BitmapLayer *bg_layer;
static TextLayer *name_layer;
static TextLayer *time_layer;
static TextLayer *date_layer;
static BitmapLayer *separator_layer;
static char time_str[] = "00:00";
static char date_str[] = "Xxxxxxxxxxxxxxxxxxxxxxx 00";

void update_screen(struct tm *tick_time) {
  char *time_format = clock_is_24h_style() ? "%R" : "%I:%M";

  strftime(time_str, sizeof(time_str), time_format, tick_time);
  strftime(date_str, sizeof(date_str), "%B %e", tick_time);


  text_layer_set_text(time_layer, time_str);
  text_layer_set_text(date_layer, date_str);

  void* layers[] = {
    bg_layer,
    name_layer,
    time_layer,
    date_layer,
    separator_layer,
  };
  window_set_user_data(window, layers);

  uint8_t color_index = DEBUG ? tick_time->tm_sec : tick_time->tm_min;
  colorize_screen(window, color_index);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  GFont font_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  GFont font_gothic_24_bold = fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD);
  GFont font_roboto_49 = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);

  bg_layer = bitmap_layer_create((GRect) {
    .origin = { 0, 0 },
    .size = { bounds.size.w, bounds.size.h }
  });
  layer_set_update_proc(bitmap_layer_get_layer(bg_layer), bg_layer_update);
  layer_add_child(window_layer, bitmap_layer_get_layer(bg_layer));

  name_layer = text_layer_create((GRect) {
    .origin = { 0, 0 },
    .size = { bounds.size.w, 46 }
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

  time_t now = time(NULL);
  struct tm *time_tick = localtime(&now);
  update_screen(time_tick);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_screen(tick_time);
  if (units_changed & HOUR_UNIT) {// && settings->hour_vibes) {
    vibes_double_pulse();
  }
}

static void window_unload(Window *window) {
  bitmap_layer_destroy(bg_layer);
  text_layer_destroy(name_layer);
  text_layer_destroy(time_layer);
  text_layer_destroy(date_layer);
  bitmap_layer_destroy(separator_layer);
  colorize_clean();
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
  app_event_loop();
  deinit();
}
