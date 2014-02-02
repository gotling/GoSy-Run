#include <pebble.h>
#include "interval_config.h"

enum _activity { WORKOUT, REST, EXTENDE_REST, FINISHED, PAUSED };

static struct IntervalUi {
	Window *window;
	TextLayer *top_text;
	TextLayer *middle_text;
	TextLayer *time_text;
	TextLayer *total_time_text;
	BitmapLayer *image;
} ui;

static struct IntervalState {
	AppTimer *timer;
	bool active;
	uint16_t round;
	uint16_t round_time;
	uint16_t total_time;
	enum _activity activity;
	enum _activity paused_activity;
} state;

static struct IntervalImages {
	GBitmap *checkmark;
} image;

static char buf[12];
static char timebuf[20];

static void update_time() {
	state.round_time--;
	state.total_time++;
}

// When changing from workout to rest or vise verse
static void update_ui() {
	if (state.activity == WORKOUT || state.activity == REST 
		|| state.activity == EXTENDE_REST) {
		snprintf(buf, 12, "Round %d/%d", state.round, interval_rounds);
		text_layer_set_text(ui.middle_text, buf);
	}
	
	switch (state.activity) {
		case WORKOUT:
			text_layer_set_text(ui.top_text, "Workout");
			break;
		case REST:
			text_layer_set_text(ui.top_text, "Rest");
			break;
		case EXTENDE_REST:
			text_layer_set_text(ui.top_text, "Extended Rest");
			break;
		case PAUSED:
			text_layer_set_text(ui.top_text, "PAUSED");
			break;
		case FINISHED:
			text_layer_set_text(ui.top_text, "You are done!");
			text_layer_set_text(ui.middle_text, "");
			text_layer_set_text(ui.time_text, "");
			bitmap_layer_set_bitmap(ui.image, image.checkmark);
			break;

		default:
			break;
	}
}

static char* format_time(int seconds) {
	char *formated_time = (char *) malloc(sizeof(char) * 7);
	if (seconds < 60) {
		snprintf(formated_time, 7, "%d", seconds);
	} else {
		snprintf(formated_time, 7, "%d:%02d", seconds / 60, (seconds % 60));
	}
	
	return formated_time;
}

// Every second
static void update_time_ui() {
	text_layer_set_text(ui.time_text, format_time(state.round_time));
	
	snprintf(timebuf, sizeof timebuf, "Total time: %s", format_time(state.total_time));
	text_layer_set_text(ui.total_time_text, timebuf);
}

static void timer_callback(void *data) {
	state.timer = app_timer_register(1000, timer_callback, NULL);

	// Switch between states 
	if (state.round_time == 0) {
		if (state.round < interval_rounds) {
			if (state.activity == WORKOUT) {
				if (state.round % interval_extended_rest_rounds != 0) {
					state.activity = REST;
					state.round_time = interval_rest_time;
				} else {
					state.activity = EXTENDE_REST;
					state.round_time = interval_extended_rest_time;
				}
				
				vibes_short_pulse();
			} else {
				state.activity = WORKOUT;
				state.round++;
				state.round_time = interval_workout_time;      
				vibes_long_pulse();
			}
		} else {
			state.activity = FINISHED;
			vibes_double_pulse();
			state.active = false;
			app_timer_cancel(state.timer);
			state.timer = NULL;
		}
		
		update_ui();
	}
	
	if (state.activity != FINISHED) {
		update_time_ui();
		update_time();
	}
}

static void start() {
	if (state.activity == PAUSED) {
		state.activity = state.paused_activity;
	}
	
	state.active = true;
	
	vibes_short_pulse();
	
	update_ui();
	
	timer_callback(NULL);
}

static void pause() {
	if (state.timer != NULL) {
		app_timer_cancel(state.timer);
		state.timer = NULL;
	}
	
	state.active = false;
	state.paused_activity = state.activity;
	state.activity = PAUSED;
	
	update_ui();
}

static void reset() {
	if (state.timer != NULL) {
		app_timer_cancel(state.timer);
		state.timer = NULL;
	}
	
	state.activity = WORKOUT;
	state.active = false;
	state.round = 1;
	state.round_time = interval_workout_time;
	
	update_ui();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
	if (state.activity != PAUSED && state.activity != FINISHED) {
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
	
	ui.middle_text = text_layer_create((GRect) { .origin = { 0, 32 }, .size = { bounds.size.w, 26 } });
	text_layer_set_text_alignment(ui.middle_text, GTextAlignmentCenter);
	text_layer_set_overflow_mode(ui.top_text, GTextOverflowModeWordWrap);
	text_layer_set_font(ui.middle_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(ui.middle_text));

	ui.time_text = text_layer_create((GRect) { .origin = { 0, 58 }, .size = { bounds.size.w, 44 } });
	text_layer_set_text_alignment(ui.time_text, GTextAlignmentCenter);
	text_layer_set_font(ui.time_text, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
	layer_add_child(window_layer, text_layer_get_layer(ui.time_text));
	
	ui.total_time_text = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 36 }, .size = { bounds.size.w, 34 } });
	text_layer_set_text_alignment(ui.total_time_text, GTextAlignmentCenter);
	text_layer_set_font(ui.time_text, fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
	layer_add_child(window_layer, text_layer_get_layer(ui.total_time_text));
	
	image.checkmark = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_CHECKMARK);
	
	GRect image_frame = (GRect) { .size = image.checkmark->bounds.size };
	image_frame.origin.x = (bounds.size.w / 2) - 32;
	image_frame.origin.y = (bounds.size.h / 2) - 32;
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