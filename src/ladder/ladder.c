#include <pebble.h>
#include "config.h"
#include "../common/tools.h"

// REPEAT, SET

enum _activity { FAST, SLOW, EXTENDED_SLOW, FINISHED, PAUSED };

static int rounds;
static int round_iterator = 0;
static int *round_time;

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

static char buf[15];

static void set_up() {
	rounds = ladder_get_step_count();

	round_time = malloc(rounds * sizeof *round_time);
	ladder_set_up(round_time);
}

static void update_time() {
	state.round_time--;
	state.total_time++;
}

static char timebuf0[7];
static char timebuf1[7];
static char timebuf2[20];

// Every second
static void update_time_ui() {
	text_layer_set_text(ui.time_text, format_time(timebuf0, state.round_time));
	
	snprintf(timebuf2, sizeof timebuf2, "Total time: %s", format_time(timebuf1, state.total_time));
	text_layer_set_text(ui.total_time_text, timebuf2);
}

// When changing from FAST to SLOW or vise verse
static void update_ui() {
	if (state.activity == FAST || state.activity == SLOW 
		|| state.activity == EXTENDED_SLOW) {
		snprintf(buf, sizeof buf, "Round %d:%d/%d:%d", (round_iterator + 1), state.round, rounds, ladder_rounds);
		text_layer_set_text(ui.middle_text, buf);
	}
	
	switch (state.activity) {
		case FAST:
			text_layer_set_text(ui.top_text, "Fast");
			break;
		case SLOW:
			text_layer_set_text(ui.top_text, "Recover");
			break;
		case EXTENDED_SLOW:
			text_layer_set_text(ui.top_text, "Extended Recovery");
			break;
		case PAUSED:
			text_layer_set_text(ui.top_text, "PAUSED");
			break;
		case FINISHED:
			text_layer_set_text(ui.top_text, "You are done!");
			text_layer_set_text(ui.middle_text, "");
			update_time_ui();
			text_layer_set_text(ui.time_text, "");
			bitmap_layer_set_bitmap(ui.image, image.checkmark);
			break;
		default:
			break;
	}
}

static void timer_callback(void *data) {
	state.timer = app_timer_register(1000, timer_callback, NULL);

	// Switch between states 
	if (state.round_time == 0) {
		if (state.round < ladder_rounds || round_iterator < (rounds - 1)) {
			if (state.activity == FAST) {
				/*if (interval_extended_slow && state.round % interval_extended_slow_rounds == 0) {
					state.activity = EXTENDED_SLOW;
					state.round_time = interval_extended_slow_time;
				} else { */
					state.activity = SLOW;
					state.round_time = ladder_slow_time;
				//}
				
				vibes_long_pulse();
			} else {
				state.activity = FAST;
				
				round_iterator++;
				
				if (round_iterator == rounds) {
					round_iterator = 0;
					state.round++;
				}
				
				state.round_time = round_time[round_iterator];

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
	} else if (state.round_time <= 3) {
		vibes_short_pulse();
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
	
	state.activity = FAST;
	state.active = false;
	state.round = 1;
	state.round_time = round_time[0];
	state.total_time = 0;
	round_iterator = 0;
	
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

	ui.time_text = text_layer_create((GRect) { .origin = { 0, 58 }, .size = { bounds.size.w, 52 } });
	text_layer_set_text_alignment(ui.time_text, GTextAlignmentCenter);
	text_layer_set_font(ui.time_text, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
	layer_add_child(window_layer, text_layer_get_layer(ui.time_text));
	
	ui.total_time_text = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 34 }, .size = { bounds.size.w, 32 } });
	text_layer_set_text_alignment(ui.total_time_text, GTextAlignmentCenter);
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
	reset();
	
	text_layer_destroy(ui.top_text);
	text_layer_destroy(ui.middle_text);
	text_layer_destroy(ui.time_text);
	text_layer_destroy(ui.total_time_text);
	bitmap_layer_destroy(ui.image);
	
	gbitmap_destroy(image.checkmark);

	if (round_time) {
		free(round_time);
	}

	window_destroy(window);
}

void ladder_init(void) {
	ui.window = window_create();
	window_set_click_config_provider(ui.window, click_config_provider);
	window_set_window_handlers(ui.window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});

	set_up();

	const bool animated = true;
	window_stack_push(ui.window, animated);
}

void ladder_deinit(void) {
	window_destroy(ui.window);
}