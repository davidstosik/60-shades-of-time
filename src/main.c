#include <pebble.h>

static Window *window;
static TextLayer *time_layer;

void update_screen() {
  time_t now = time(NULL);
  struct tm * now_tm = localtime(&now);

  text_layer_set_text(time_layer, "Press a button");

  GColor8 bg = (GColor8){.argb = (0b11000000 + now_tm->tm_sec)};
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Updating screen, color: %d", bg.argb);

  window_set_background_color(window, bg);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  time_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
  text_layer_set_background_color(time_layer, GColorClear);
  layer_add_child(window_layer, text_layer_get_layer(time_layer));

  update_screen();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_screen();
  if (units_changed & HOUR_UNIT) {// && settings->hour_vibes) {
    vibes_double_pulse();
  }
}

static void window_unload(Window *window) {
  text_layer_destroy(time_layer);
}

static void init(void) {
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);

  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);
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
