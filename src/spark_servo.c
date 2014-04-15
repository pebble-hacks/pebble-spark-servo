#include <pebble.h>

static Window *window;
static TextLayer *text_layer;
static char g_text[100];
static AppTimer *timer;

static AppSync  gSync;
static uint8_t  gSyncBuffer[64];


// NOTE: The javascript code references these same constants through the strings
//  defined in the "appKeys" section of appinfo.json
enum InMessageKey {
  SERVO_KEY = 0x0,            // TUPLE_INT
};


static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}


static void timer_callback(void *data) {
  AccelData accel = (AccelData) { .x = 0, .y = 0, .z = 0 };

  accel_service_peek(&accel);
  snprintf(g_text, sizeof(g_text), "%d %d %d", accel.x, accel.y, accel.z);
  APP_LOG(APP_LOG_LEVEL_DEBUG, "servo position: %d %d %d", accel.x, accel.y, accel.z);

  text_layer_set_text(text_layer, g_text);
  layer_mark_dirty(text_layer_get_layer(text_layer));

  // Update servo value
  int servo_pos = (accel.y + 1000) * 180/2000;
  if (servo_pos < 0) {
    servo_pos = 0;
  } else if (servo_pos > 180) {
    servo_pos = 180;
  }
  Tuplet value = TupletInteger(SERVO_KEY, servo_pos);
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  if (iter != NULL) {
    dict_write_tuplet(iter, &value);
    dict_write_end(iter);
    app_message_outbox_send();
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Updated server position to: %d", servo_pos);
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Could not write to outbox");
  }

  timer = app_timer_register(330 /* milliseconds */, timer_callback, NULL);
}


/*****************************************************************************/
static void sync_error_callback(DictionaryResult dict_error, AppMessageResult app_message_error, void *context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "App Message Sync Error: %d", app_message_error);
}

static void sync_tuple_changed_callback(const uint32_t key, 
        const Tuple* new_tuple, const Tuple* old_tuple, void* context) {
  APP_LOG(APP_LOG_LEVEL_DEBUG, "sync_tuple_changed");
}


static void handle_accel(AccelData *accel_data, uint32_t num_samples) {
  // do nothing
}


static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);

  accel_data_service_subscribe(0, handle_accel);
  timer = app_timer_register(100 /* milliseconds */, timer_callback, NULL);


  // Init app messages
  const int inbound_size = 64;
  const int outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
  
  Tuplet initialValues[] = {
    TupletInteger(SERVO_KEY, (int32_t)90),
  };

  app_sync_init(&gSync, gSyncBuffer, sizeof(gSyncBuffer), initialValues, 
              ARRAY_LENGTH(initialValues), sync_tuple_changed_callback, 
              sync_error_callback, NULL);
}


static void deinit(void) {
  accel_data_service_unsubscribe();
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
