#include <pebble.h>
#include "stretch.h"
#include "interval.h"
#include "interval_config.h"
#include "interval_config_menu.h"
#include "stretch_config.h"
#include "stretch_config_menu.h"
#include "tools.h"

#define NUM_MENU_SECTIONS 2
#define NUM_FIRST_MENU_ITEMS 2
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
			menu_cell_basic_header_draw(ctx, cell_layer, "Interval Timer");
			break;
		case 1:
			menu_cell_basic_header_draw(ctx, cell_layer, "Stretch Timer");
			break;
	}
}

static char subbuf[25];

static char *interval_subtitle(char *subtitle) {
	char workout_time_text[7];
	format_time(workout_time_text, interval_workout_time);
	
	char rest_time_text[7];
	format_time(rest_time_text, interval_rest_time);
	
	snprintf(subbuf, sizeof subbuf, "%s+%s * %d", workout_time_text, rest_time_text, interval_rounds);
	
	if (interval_extended_rest) {
		char extended_rest_time_text[7];
		format_time(extended_rest_time_text, interval_extended_rest_time);
		
		char erbuf[10];
		snprintf(erbuf, sizeof erbuf, " +%s/%d", extended_rest_time_text, interval_extended_rest_rounds);
		strncat(subbuf, erbuf, sizeof erbuf);
	}
	
	return subtitle;
}

static char *stretch_subtitle(char *subtitle) {
	char stretch_time_text[7];
	format_time(stretch_time_text, stretch_stretch_time);
	
	char prepare_time_text[7];
	format_time(prepare_time_text, stretch_prepare_time);
	
	snprintf(subbuf, sizeof subbuf, "%s+%s", stretch_time_text, prepare_time_text);
	
	return subtitle;
}

static void menu_draw_row_callback(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *data) {
	switch (cell_index->section) {
		case 0:
			switch (cell_index->row) {
				case 0:
					interval_subtitle(subbuf);
					menu_cell_basic_draw(ctx, cell_layer, "Start", subbuf, NULL);
					break;
				case 1:
					menu_cell_basic_draw(ctx, cell_layer, "Configure", NULL, NULL);
					break;
					
				default:
					break;
			}
			break;
		case 1:
			switch (cell_index->row) {
				case 0:
					stretch_subtitle(subbuf);
					menu_cell_basic_draw(ctx, cell_layer, "Start", subbuf, NULL);
					break;
				case 1:
					menu_cell_basic_draw(ctx, cell_layer, "Configure", NULL, NULL);
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
					interval_init();
					break;
					
				case 1:
					interval_config_menu_init();
					break;
			}
			break;
		case 1:
			switch (cell_index->row) {
				case 0:
					stretch_init();
					break;
				case 1:
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
	text_layer_set_text(header, "GoSy Run");
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
	text_layer_destroy(header);
	menu_layer_destroy(menu_layer);
}

void menu_init(void) {
	window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	
	interval_read_persistent();
	stretch_read_persistent();
	
	const bool animated = true;
	window_stack_push(window, animated);
}

void menu_deinit(void) {
	interval_write_persistent();
	stretch_write_persistent();
	
	window_destroy(window);
}