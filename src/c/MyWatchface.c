#include <pebble.h>
#include <stdbool.h>

static Window *s_window;
static TextLayer *s_hour_layer;
static TextLayer *s_minute_layer;
static TextLayer *s_message_layer;
static GFont s_font;

const int FONT_HEIGHT = 40;

static void prv_window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_hour_layer = text_layer_create(GRect(0, 0, bounds.size.w, FONT_HEIGHT));
  s_minute_layer = text_layer_create(GRect(0, FONT_HEIGHT, bounds.size.w, FONT_HEIGHT));
  s_message_layer = text_layer_create(GRect(0, bounds.size.h / 2, bounds.size.w, bounds.size.h / 2));

  text_layer_set_text(s_message_layer, "Hello World!!!");
  text_layer_set_text_alignment(s_hour_layer, GTextAlignmentRight);
  text_layer_set_text_alignment(s_minute_layer, GTextAlignmentRight);

  text_layer_set_text_alignment(s_message_layer, GTextAlignmentCenter);

  text_layer_set_font(s_message_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_font(s_hour_layer, s_font);
  text_layer_set_font(s_minute_layer, s_font);

  layer_add_child(window_layer, text_layer_get_layer(s_hour_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_minute_layer));
  layer_add_child(window_layer, text_layer_get_layer(s_message_layer));
}

static void prv_window_unload(Window *window)
{
  text_layer_destroy(s_hour_layer);
  text_layer_destroy(s_minute_layer);
  text_layer_destroy(s_message_layer);
}

static void handle_minute_tick(struct tm *tick_time, TimeUnits units_changed)
{
  static char s_hour_text[] = "00";
  strftime(s_hour_text, sizeof(s_hour_text), "%H", tick_time);
  text_layer_set_text(s_hour_layer, s_hour_text);

  static char s_minute_text[] = "00";
  strftime(s_minute_text, sizeof(s_minute_text), "%M", tick_time);
  text_layer_set_text(s_minute_layer, s_minute_text);
}

static void prv_init(void)
{
  s_window = window_create();
  s_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_MY_FONT_FACE_34));

  window_set_window_handlers(s_window, (WindowHandlers){
                                           .load = prv_window_load,
                                           .unload = prv_window_unload,
                                       });
  const bool animated = true;
  window_stack_push(s_window, animated);

  tick_timer_service_subscribe(MINUTE_UNIT, handle_minute_tick);
}

static void prv_deinit(void)
{
  window_destroy(s_window);

  tick_timer_service_unsubscribe();
  fonts_unload_custom_font(s_font);
}

int main(void)
{
  prv_init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", s_window);

  app_event_loop();
  prv_deinit();
}
