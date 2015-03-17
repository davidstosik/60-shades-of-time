#include <pebble.h>

#ifndef PBL_COLOR

static char* color_names[] = {
  "Black",
  "White",
  "Gray 75%",
  "Gray 50%",
  "Gray 25%",
};

static GColor gcolor_fgcolor(uint8_t bgcolor_index) {
  switch(bgcolor_index) {
    case GColorWhite:
    case 2:
    case 3:
      return GColorBlack;
    case GColorBlack:
    default:
      return GColorWhite;
  }
}

static GBitmap *all_bg;
static GBitmap *bg_bitmaps[5];
static uint8_t bg_color_index;

void bg_layer_update(Layer *bg_layer, GContext *ctx) {
  if(!all_bg) {
    all_bg = gbitmap_create_with_resource(RESOURCE_ID_BW_BG);
    for (int i = 0; i < 5; i++) {
      bg_bitmaps[i] = gbitmap_create_as_sub_bitmap(all_bg, GRect(2*i, 0, 2, 2));
    }
  }
  graphics_draw_bitmap_in_rect(ctx, bg_bitmaps[bg_color_index], GRect(0,0,144,168));
}

void colorize_screen(Window *window, uint8_t color_index) {
  void** layers = window_get_user_data(window);
  Layer *bg_layer = layers[0];
  TextLayer *name_layer = layers[1];
  TextLayer *time_layer = layers[2];
  TextLayer *date_layer = layers[3];
  BitmapLayer *separator_layer = layers[4];

  bg_color_index = color_index%5;

  text_layer_set_text(name_layer, color_names[bg_color_index]);

  GColor fg_color = gcolor_fgcolor(bg_color_index);
  text_layer_set_text_color(name_layer, fg_color);
  text_layer_set_text_color(time_layer, fg_color);
  text_layer_set_text_color(date_layer, fg_color);
  bitmap_layer_set_background_color(separator_layer, fg_color);

  layer_mark_dirty(bg_layer);
}

void colorize_clean() {
  gbitmap_destroy(all_bg);
  for (int i = 0; i < 5; i++) {
    gbitmap_destroy(bg_bitmaps[i]);
  }
}

#else

static char* color_names[] = {
  "Oxford Blue",
  "Duke Blue",
  "Blue",
  "Dark Green\n(X11)",
  "Midnight Green\n(Eagle Green)",
  "Cobalt Blue",
  "Blue Moon",
  "Islamic Green",
  "Jaeger Green",
  "Tiffany Blue",
  "Vivid Cerulean",
  "Green",
  "Malachite",
  "Medium Spring Green",
  "Cyan",
  "Bulgarian Rose",
  "Imperial Purple",
  "Indigo (Web)",
  "Electric Ultramarine",
  "Army Green",
  "Liberty",
  "Very Light Blue",
  "Kelly Green",
  "May Green",
  "Cadet Blue",
  "Picton Blue",
  "Bright Green",
  "Screamin' Green",
  "Medium Aquamarine",
  "Electric Blue",
  "Dark Candy Apple Red",
  "Jazzberry Jam",
  "Purple",
  "Vivid Violet",
  "Windsor Tan",
  "Rose Vale",
  "Purpureus",
  "Lavender Indigo",
  "Limerick",
  "Brass",
  "Baby Blue Eyes",
  "Spring Bud",
  "Inchworm",
  "Mint Green",
  "Celeste",
  "Red",
  "Folly",
  "Fashion Magenta",
  "Magenta",
  "Orange",
  "Sunset Orange",
  "Brilliant Rose",
  "Shocking Pink\n(Crayola)",
  "Chrome Yellow",
  "Rajah",
  "Melon",
  "Rich Brilliant Lavender",
  "Yellow",
  "Icterine",
  "Pastel Yellow",
};

static GColor8 bg_color;

void bg_layer_update(Layer *layer, GContext *ctx) {
  graphics_context_set_fill_color(ctx, bg_color);
  graphics_fill_rect(ctx, layer_get_frame(layer), 0, GCornerNone);
}

static GColor get_color_from_minute(uint8_t minute) {
  uint8_t argb = GColorBlackARGB8 + minute + 1;

  if (argb >= GColorDarkGrayARGB8) { argb++; }
  if (argb >= GColorLightGrayARGB8) { argb++; }

  return (GColor8){ .argb = argb };
}

static GColor gcolor_fgcolor(GColor bgcolor) {
  // Inspiration: http://stackoverflow.com/a/946734/2341409
  uint8_t r, g, b;
  r = (bgcolor.argb & 0b110000) >> 4;
  g = (bgcolor.argb & 0b001100) >> 2;
  b = bgcolor.argb & 0b000011;
  uint16_t brightness = r*299 + g*587 + b*114;
  return brightness < 1750 ? GColorWhite : GColorBlack;
}

void colorize_screen(Window *window, uint8_t color_index) {
  void** layers = window_get_user_data(window);
  Layer *bg_layer = layers[0];
  TextLayer *name_layer = layers[1];
  TextLayer *time_layer = layers[2];
  TextLayer *date_layer = layers[3];
  BitmapLayer *separator_layer = layers[4];

  text_layer_set_text(name_layer, color_names[color_index]);

  bg_color = get_color_from_minute(color_index);
  layer_mark_dirty(bg_layer);

  GColor fg_color = gcolor_fgcolor(bg_color);
  text_layer_set_text_color(name_layer, fg_color);
  text_layer_set_text_color(time_layer, fg_color);
  text_layer_set_text_color(date_layer, fg_color);
  bitmap_layer_set_background_color(separator_layer, fg_color);
}

void colorize_clean() {}

#endif // ifndef PBL_COLOR
