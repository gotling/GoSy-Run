#pragma once

char *format_time(char *formated_time, int seconds);
char *format_time_long(char *formated_time, int seconds);
void menu_cell_basic_draw_multiline(GContext* ctx, const Layer *cell_layer, char *title, char *subtitle, GBitmap *icon);