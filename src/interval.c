#include <pebble.h>
#include "interval_config.h"

static struct IntervalUi {
	Window *window;
	TextLayer *top_text;
	TextLayer *middle_text;
	TextLayer *time_text;
	BitmapLayer *image;
} ui;

static struct IntervalState {
	AppTimer *timer;
	uint16_t running;
	uint16_t round;
	uint16_t round_time;
	bool workout;
} state;

static struct IntervalImages {
	GBitmap *checkmark;
} image;

static char buf[12];
static char timebuf[12];

static void update_ui() {
	if (state.round < interval_rounds) {
		snprintf(buf, 12, "Round %d/%d", state.round + 1, interval_rounds);
		text_layer_set_text(ui.middle_text, buf);
		
		snprintf(timebuf, 12, "%d", state.round_time);
		text_layer_set_text(ui.time_text, timebuf);
	} else {
		text_layer_set_text(ui.top_text, "You are done!");
		text_layer_set_text(ui.middle_text, "");
		text_layer_set_text(ui.time_text, "");
		
		bitmap_layer_set_bitmap(ui.image, image.checkmark);
		
		vibes_double_pulse();
		state.running = 0;
		app_timer_cancel(state.timer);
		state.timer = NULL;
	}
}

static void timer_callback(void *data) {
	state.timer = app_timer_register(1000, timer_callback, NULL);
	
	if (state.round_time == 0) {
		if (state.workout) {
			if ((state.round + 1 ) % interval_extended_rest_rounds != 0) {
				text_layer_set_text(ui.top_text, "Rest");
				state.round_time = interval_rest_time;
			} else {
				text_layer_set_text(ui.top_text, "Extended Rest");
				state.round_time = interval_extended_rest_time;
			}
			
			vibes_short_pulse();
			state.workout = false;
		} else {
			state.round++;
			state.round_time = interval_workout_time;      
			vibes_long_pulse();
			text_layer_set_text(ui.top_text, "Workout");
			state.workout = true;
		}
	}
	
	update_ui();
	state.round_time--;
}

static void start() {
	state.running = 1;
	
	vibes_short_pulse();
	
	if (state.workout) {
		text_layer_set_text(ui.top_text, "Workout");
	} else {
		text_layer_set_text(ui.top_text, "Rest");
	}
	
	timer_callback(NULL);
}

static void pause() {
	if (state.timer != NULL) {
		app_timer_cancel(state.timer);
		state.timer = NULL;
	}
	
	state.running = 0;
	
	text_layer_set_text(ui.top_text, "PAUSED");
}

static void reset() {
	if (state.timer != NULL) {
		app_timer_cancel(state.timer);
		state.timer = NULL;
	}
	
	state.running = 0;
	state.round = 0;
	state.round_time = interval_workout_time;
	state.workout = true;
	
	update_ui();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (state.running) {
		pause();
	} else {
		start();
	}
}

static void click_config_provider(void *context) {
	window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	
	ui.top_text = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 32 } });
	text_layer_set_text_alignment(ui.top_text, GTextAlignmentCenter);
	text_layer_set_font(ui.top_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(ui.top_text));
	
	ui.middle_text = text_layer_create((GRect) { .origin = { 0, 32 }, .size = { bounds.size.w, 52 } });
	text_layer_set_text_alignment(ui.middle_text, GTextAlignmentCenter);
	text_layer_set_overflow_mode(ui.top_text, GTextOverflowModeWordWrap);
	text_layer_set_font(ui.middle_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(ui.middle_text));

	ui.time_text = text_layer_create((GRect) { .origin = { 5, 84 }, .size = { 80, 49 } });
	text_layer_set_text_alignment(ui.time_text, GTextAlignmentCenter);
	text_layer_set_font(ui.time_text, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(window_layer, text_layer_get_layer(ui.time_text));
	
	image.checkmark = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHECKMARK);
	
	GRect image_frame = (GRect) { .size = image.checkmark->bounds.size };
	image_frame.origin.x = 72;
	image_frame.origin.y = 84;
	ui.image = bitmap_layer_create(image_frame);
	layer_add_child(window_layer, bitmap_layer_get_layer(ui.image));
	
	reset();
	start();
}

static void window_unload(Window *window) {
	text_layer_destroy(ui.top_text);
	text_layer_destroy(ui.middle_text);
	text_layer_destroy(ui.time_text);	
	bitmap_layer_destroy(ui.image);
	
	gbitmap_destroy(image.checkmark);
	
	reset();
	window_destroy(window);
}

void interval_init(void) {
	ui.window = window_create();
	window_set_click_config_provider(ui.window, click_config_provider);
	window_set_window_handlers(ui.window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	const bool animated = true;
	window_stack_push(ui.window, animated);
}

void interval_deinit(void) {
	window_destroy(ui.window);
}
