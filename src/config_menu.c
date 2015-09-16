#include <pebble.h>
#include "stretch/stretch.h"
#include "stretch/config.h"
#include "stretch/stretch_config_menu.h"
#include "interval/interval.h"
#include "interval/interval_config.h"
#include "interval/interval_config_menu.h"
#include "ladder/ladder.h"
#include "ladder/config.h"
#include "ladder/config_menu.h"
#include "common/tools.h"

#define NUM_MENU_SECTIONS 0
#define NUM_FIRST_MENU_ITEMS 3

static Window *window;
static TextLayer *header;
static MenuLayer *menu_layer;
void (*callback_function)(void);

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:
			return NUM_FIRST_MENU_ITEMS;
		default:
			return 0;
	}
}

static int16_t menu_get_cell_height_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
	return MENU_CELL_BASIC_HEIGHT;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					menu_cell_basic_draw(ctx, cell_layer, "Interval", NULL, NULL);
					break;
				case 1:
					menu_cell_basic_draw(ctx, cell_layer, "Ladder", NULL, NULL);
					break;
				case 2:
					menu_cell_basic_draw(ctx, cell_layer, "Stretch", NULL, NULL);
					break;
				default:
					break;
			}
			break;
	}
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					interval_config_menu_init();
					break;
				case 1:
					ladder_config_menu_init();
					break;
				case 2:
					stretch_config_menu_init();
					break;
			}
		break;
	}
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);

	header = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 32 } });
	text_layer_set_text(header, "Settings");
	text_layer_set_text_alignment(header, GTextAlignmentCenter);
	text_layer_set_font(header, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(header));
	
	menu_layer = menu_layer_create((GRect) { .origin = { 0, 40 }, .size = { bounds.size.w, MENU_HEIGHT } });
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_cell_height = menu_get_cell_height_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	menu_layer_set_click_config_onto_window(menu_layer, window);
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}


static void window_unload(Window *window) {
	text_layer_destroy(header);
	menu_layer_destroy(menu_layer);

	callback_function();

	window_destroy(window);
}

void config_menu_init(void (*callback)(void)) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	const bool animated = true;
	callback_function = callback;
	window_stack_push(window, animated);
}

void config_menu_deinit(void) {
	window_destroy(window);
}