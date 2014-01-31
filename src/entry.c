#include <pebble.h>
#include "entry.h"
#include "interval_config.h"

#define STEP 5

static struct EntryUi {
	Window *window;
	TextLayer *title_text;
	char *title;
	TextLayer *entry_text;
} ui;

static struct EntryState {
	int entry;
} state;

static char buf[6];

static void up_click_handler(ClickRecognizerRef recognizer, void* context) {
	state.entry += STEP;
	snprintf(buf, 6, "%d", state.entry);
	text_layer_set_text(ui.entry_text, buf);
	layer_mark_dirty(text_layer_get_layer(ui.entry_text));
}

static void down_click_handler(ClickRecognizerRef recognizer, void* context) {
	if (state.entry > 0) {
		state.entry -= STEP;
		snprintf(buf, 6, "%d", state.entry);
		text_layer_set_text(ui.entry_text, buf);
		layer_mark_dirty(text_layer_get_layer(ui.entry_text));
	}
}

//static void select_click_handler(ClickRecognizerRef recognizer, void* context) {
//}

static void click_config_provider(void* context) {
	const uint16_t repeat_interval_ms = 100;
	window_set_click_context(BUTTON_ID_UP, context);
	window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, up_click_handler);
	
	//window_set_click_context(BUTTON_ID_SELECT, context);
	//window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
	
	window_set_click_context(BUTTON_ID_DOWN, context);
	window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, down_click_handler);
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_bounds(window_layer);
	
	ui.title_text = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 32 }});
	text_layer_set_text(ui.title_text, ui.title);
	text_layer_set_text_alignment(ui.title_text, GTextAlignmentCenter);
	text_layer_set_font(ui.title_text, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(ui.title_text));
	
	state.entry = workout;
	snprintf(buf, 6, "%d", state.entry);
	
	ui.entry_text = text_layer_create((GRect) { .origin = { 0, 64 }, .size = { bounds.size.w, 28 }});
	text_layer_set_text(ui.entry_text, buf);
	text_layer_set_text_alignment(ui.entry_text, GTextAlignmentCenter);
	text_layer_set_font(ui.entry_text, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(ui.entry_text));
}

static void window_unload(Window *window) {
	text_layer_destroy(ui.title_text);
	text_layer_destroy(ui.entry_text);

	workout = state.entry;
}

void entry_init(void) {
	ui.window = window_create();
	window_set_click_config_provider(ui.window, click_config_provider);
	window_set_window_handlers(ui.window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	const bool animated = true;
	window_stack_push(ui.window, animated);
}

void entry_init2(char *title) {
	ui.title = title;
	entry_init();
}

void entry_deinit(void) {
	window_destroy(ui.window);
}
