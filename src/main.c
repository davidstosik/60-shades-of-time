#include <pebble.h>
#include "color_names.h"

static Window *window;
static TextLayer *name_layer;
static TextLayer *time_layer;
static GFont font_gothic_18;
static GFont font_roboto_49;

static GColor8 get_color_from_minute(uint8_t minute) {
  uint8_t argb = GColorBlackARGB8 + minute + 1;

  if (argb >= GColorDarkGrayARGB8) { argb++; }
  if (argb >= GColorLightGrayARGB8) { argb++; }

  return (GColor8){ .argb = argb };
}

static GColor8 gcolor_fgcolor(GColor8 bgcolor) {
  uint8_t r, g, b;
  r = (bgcolor.argb & 0b110000) >> 4;
  g = (bgcolor.argb & 0b001100) >> 2;
  b = bgcolor.argb & 0b000011;
  return r+g+b < 5 ? GColorWhite : GColorBlack;
}

void update_screen(struct tm *tick_time) {
  char *time_str = "00:00";
  clock_copy_time_string(time_str, 6);

  text_layer_set_text(name_layer, color_names[tick_time->tm_min]);
  text_layer_set_text(time_layer, time_str);

  GColor8 bg = get_color_from_minute(tick_time->tm_min);
  window_set_background_color(window, bg);

  text_layer_set_text_color(name_layer, gcolor_fgcolor(bg));
  text_layer_set_text_color(time_layer, gcolor_fgcolor(bg));
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  font_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  font_roboto_49 = fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49);

  uint8_t bottom_margin = 5;
  uint8_t height = 20;

  name_layer = text_layer_create((GRect) {
    .origin = { 0, (bounds.size.h - height - bottom_margin) },
    .size = { bounds.size.w, (height + bottom_margin) }
  });
  text_layer_set_text_alignment(name_layer, GTextAlignmentCenter);
  text_layer_set_overflow_mode(name_layer, GTextOverflowModeWordWrap);
  text_layer_set_font(name_layer, font_gothic_18);
  text_layer_set_background_color(name_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(name_layer));

  height = 70;
  time_layer = text_layer_create((GRect) {
    .origin = { 0, (bounds.size.h - height)/2 },
    .size = { bounds.size.w, height }
  });
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_font(time_layer, font_roboto_49);
  text_layer_set_background_color(time_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));
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
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
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
