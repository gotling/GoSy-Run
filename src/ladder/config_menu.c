#include <pebble.h>
#include "config.h"
#include "../common/entry.h"
#include "../common/tools.h"

#define NUM_MENU_SECTIONS 1
#define NUM_FIRST_MENU_ITEMS 5
#define NUM_SECOND_MENU_ITEMS 1

static Window *window;
static TextLayer *header;
static MenuLayer *menu_layer;
static int second_menu_items = NUM_SECOND_MENU_ITEMS;

static uint16_t menu_get_num_sections_callback(MenuLayer *menu_layer, void *data) {
	return NUM_MENU_SECTIONS;
}

static uint16_t menu_get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *data) {
	switch (section_index) {
		case 0:
			return NUM_FIRST_MENU_ITEMS;
		case 1:
			if (ladder_extended_slow) {
				return 3;
			} else {
				return 1;
			}
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
			menu_cell_basic_header_draw(ctx, cell_layer, "General");
			break;
		case 1:
			menu_cell_basic_header_draw(ctx, cell_layer, "Extended Recovery");
			break;
	}
}

static char subbuf[12];

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					ladder_direction_to_string(subbuf, ladder_direction);
					menu_cell_basic_draw(ctx, cell_layer, "Direction", subbuf, NULL);
					break;
				case 1:
					format_time_long(subbuf, ladder_step_time);
					menu_cell_basic_draw(ctx, cell_layer, "Shortest", subbuf, NULL);
					break;
				case 2:
					format_time_long(subbuf, ladder_max_time);
					menu_cell_basic_draw(ctx, cell_layer, "Longest", subbuf, NULL);
					break;
				case 3:
					format_time_long(subbuf, ladder_slow_time);
					menu_cell_basic_draw(ctx, cell_layer, "Recover", subbuf, NULL);
					break;
				case 4:
					snprintf(subbuf, 12, "%d", ladder_rounds);
					menu_cell_basic_draw(ctx, cell_layer, "Repeat", subbuf, NULL);
					break;
			}
			break;
		case 1:
			switch (cell_index->row) {
				case 0:
					if (ladder_extended_slow) {
						menu_cell_basic_draw(ctx, cell_layer, "Enabled", NULL, NULL);
					} else {
						menu_cell_basic_draw(ctx, cell_layer, "Disabled", NULL, NULL);
					}
					break;
				case 1:
					format_time_long(subbuf, ladder_extended_slow_time);
					menu_cell_basic_draw(ctx, cell_layer, "Recover", subbuf, NULL);
					break;
				case 2:
					snprintf(subbuf, 12, "%d repeats", ladder_extended_slow_rounds);
					menu_cell_basic_draw(ctx, cell_layer, "Every", subbuf, NULL);
					break;
			}
			break;
	}
}

static void step_updated(void) {
	if (ladder_max_time < ladder_step_time) {
		ladder_max_time = ladder_step_time;
	} else {
		ladder_max_time -= (ladder_max_time % ladder_step_time);	
	}
}

static void menu_select_callback(MenuLayer *menu_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					entry_init_enum("Direction", &ladder_direction_to_string, 4, (int*)&ladder_direction);
					break;
				case 1:
					entry_init_time_callback("Shortest", &ladder_step_time, &step_updated);
					break;
				case 2:
					entry_init_time_step("Longest", ladder_step_time, &ladder_max_time);
					break;
				case 3:
					entry_init_time("Recover", &ladder_slow_time);
					break;
				case 4:
					entry_init_number("Repeat", "%d times", 1, &ladder_rounds);
					break;
			}
			break;
		case 1:
			switch (cell_index->row) {
				case 0:
					if (ladder_extended_slow) {
						ladder_extended_slow = false;
						second_menu_items = 1;
					} else {
						ladder_extended_slow = true;
						second_menu_items = 3;
					}
					menu_layer_reload_data(menu_layer);
					menu_layer_set_selected_index(menu_layer, menu_layer_get_selected_index(menu_layer), MenuRowAlignCenter, true);
					break;
				case 1:
					entry_init_time("Extended Recover", &ladder_extended_slow_time);
					break;
				case 2:
					entry_init_number("Extended Every", "%d repeats", 1, &ladder_extended_slow_rounds);
					break;
				default:
					break;
			}
			break;

	}
}

static void window_load(Window *window) {
	Layer *window_layer = window_get_root_layer(window);
	GRect bounds = layer_get_frame(window_layer);
	
	header = text_layer_create((GRect) { .origin = { 0, 0 }, .size = { bounds.size.w, 32 } });
	text_layer_set_text(header, "Ladder Config");
	text_layer_set_text_alignment(header, GTextAlignmentCenter);
	text_layer_set_font(header, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
	layer_add_child(window_layer, text_layer_get_layer(header));
	
	menu_layer = menu_layer_create((GRect) { .origin = { 0, 40 }, .size = { bounds.size.w, 114 } });
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
	ladder_write_persistent();
	
	text_layer_destroy(header);
	menu_layer_destroy(menu_layer);
	
	window_destroy(window);
}

void ladder_config_menu_init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	const bool animated = true;
	window_stack_push(window, animated);
}

void ladder_config_menu_deinit(void) {
	window_destroy(window);
}