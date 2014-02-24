#include <pebble.h>
#include "tools.h"

char *format_time(char *formated_time, int seconds) {
	if (seconds < 60) {
		snprintf(formated_time, 3, "%d", seconds);
	} else {
		snprintf(formated_time, 7, "%d:%02d", seconds / 60, (seconds % 60));
	}
	
	return formated_time;
}

char *format_time_long(char *formated_time, int seconds) {
	if (seconds < 60) {
		snprintf(formated_time, 11, "%d seconds", seconds);
	} else {
		snprintf(formated_time, 7, "%d:%02d", seconds / 60, (seconds % 60));
	}
	
	return formated_time;
}

void menu_cell_basic_draw_multiline(GContext* ctx, const Layer *cell_layer, char *title, char *subtitle, GBitmap *icon) {
	GRect bounds = layer_get_frame(cell_layer);
	graphics_context_set_text_color	(ctx, GColorBlack);
	graphics_draw_text(ctx,
		title,
		fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD),
		GRect(5, -5, bounds.size.w-10, 26),
		GTextOverflowModeWordWrap,
		GTextAlignmentLeft,
		NULL);
	graphics_draw_text(ctx,
		subtitle,
		fonts_get_system_font(FONT_KEY_GOTHIC_18),
		GRect(5, 21, bounds.size.w-10, 40),
		GTextOverflowModeWordWrap,
		GTextAlignmentLeft,
		NULL);
}