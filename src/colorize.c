#include <pebble.h>

#ifndef PBL_COLOR

static char* color_names[] = {
  "Black",
  "White",
};

static GColor get_color_from_minute(uint8_t minute) {
  return (GColor)(minute%2);
}

static GColor gcolor_fgcolor(GColor bgcolor) {
  return 1 - bgcolor;
}

void colorize_screen(Window *window, uint8_t color_index) {
  void** layers = window_get_user_data(window);
  TextLayer *name_layer = layers[0];
  TextLayer *time_layer = layers[1];
  TextLayer *date_layer = layers[2];
  BitmapLayer *separator_layer = layers[3];

  text_layer_set_text(name_layer, color_names[color_index%2]);

  GColor bg = get_color_from_minute(color_index);
  window_set_background_color(window, bg);

  GColor fg_color = gcolor_fgcolor(bg);
  text_layer_set_text_color(name_layer, fg_color);
  text_layer_set_text_color(time_layer, fg_color);
  text_layer_set_text_color(date_layer, fg_color);
  bitmap_layer_set_background_color(separator_layer, fg_color);
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
  TextLayer *name_layer = layers[0];
  TextLayer *time_layer = layers[1];
  TextLayer *date_layer = layers[2];
  BitmapLayer *separator_layer = layers[3];

  text_layer_set_text(name_layer, color_names[color_index]);

  GColor8 bg = get_color_from_minute(color_index);
  window_set_background_color(window, bg);

  GColor fg_color = gcolor_fgcolor(bg);
  text_layer_set_text_color(name_layer, fg_color);
  text_layer_set_text_color(time_layer, fg_color);
  text_layer_set_text_color(date_layer, fg_color);
  bitmap_layer_set_background_color(separator_layer, fg_color);
}

#endif // ifndef PBL_COLOR
