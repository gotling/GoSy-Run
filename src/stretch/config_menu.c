#include <pebble.h>
#include "config.h"
#include "../common/entry.h"
#include "../common/tools.h"
#include "../common/storage.h"

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 1
#define NUM_SECOND_MENU_ITEMS 2

static Window *window;
static TextLayer *header;
static MenuLayer *menu_layer;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:
			return NUM_FIRST_MENU_ITEMS;
		case 1:
			return NUM_SECOND_MENU_ITEMS;
		default:
			return 0;
	}
}

static int16_t menu_get_header_height_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	return MENU_CELL_BASIC_HEADER_HEIGHT;
}

static void menu_draw_header_callback(GContext* ctx, const Layer *cell_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:
			menu_cell_basic_header_draw(ctx, cell_layer, "Mode");
			break;
		case 1:
			menu_cell_basic_header_draw(ctx, cell_layer, "Times");
			break;
	}
}

static char subbuf[12];

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					if (stretch_settings.program) {
						menu_cell_basic_draw(ctx, cell_layer, "Program", NULL, NULL);
					} else {
						menu_cell_basic_draw(ctx, cell_layer, "Free", NULL, NULL);
					}
					break;
			}
			break;
		case 1:
			switch (cell_index->row) {
				case 0:
					format_time_long(subbuf, stretch_settings.time);
					menu_cell_basic_draw(ctx, cell_layer, "Stretch", subbuf, NULL);
					break;
				case 1:
					format_time_long(subbuf, stretch_settings.prepare);
					menu_cell_basic_draw(ctx, cell_layer, "Prepare", subbuf, NULL);
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
					if (stretch_settings.program) {
						stretch_settings.program = false;
					} else {
						stretch_settings.program = true;
					}
					menu_layer_reload_data(menu_layer);
					break;
			}
			break;
		case 1:
			switch (cell_index->row) {
				case 0:
					entry_init_time("Stretch", &stretch_settings.time);
					break;
				case 1:
					entry_init_time_zero_allowed("Prepare", &stretch_settings.prepare);
					break;
			}
			break;
	}
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	header = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 32 } });
	text_layer_set_text(header, "Stretch Config");
	text_layer_set_text_alignment(header, GTextAlignmentCenter);
	text_layer_set_font(header, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(header));
	
	menu_layer = menu_layer_create((GRect) { .origin = { 0, 40 }, .size = { bounds.size.w, MENU_HEIGHT } });
	menu_layer_set_callbacks(menu_layer, NULL, (MenuLayerCallbacks){
		.get_num_sections = menu_get_num_sections_callback,
		.get_num_rows = menu_get_num_rows_callback,
		.get_header_height = menu_get_header_height_callback,
		.draw_header = menu_draw_header_callback,
		.draw_row = menu_draw_row_callback,
		.select_click = menu_select_callback,
	});
	menu_layer_set_click_config_onto_window(menu_layer, window);
	layer_add_child(window_layer, menu_layer_get_layer(menu_layer));
}

static void window_unload(Window *window) {
	persist_stretch_write();
	
	text_layer_destroy(header);
	menu_layer_destroy(menu_layer);
	
	window_destroy(window);
}

void stretch_config_menu_init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	const bool animated = true;
	window_stack_push(window, animated);
}

void stretch_config_menu_deinit(void) {
	window_destroy(window);
}