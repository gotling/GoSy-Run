#pragma once

extern int interval_workout_time;
extern int interval_rest_time;
extern int interval_rounds;
extern bool interval_extended_rest;
extern int interval_extended_rest_time;
extern int interval_extended_rest_rounds;
extern int interval_warm_up;
extern int interval_cool_down;

void interval_read_persistent();
void interval_write_persistent();

char *interval_tostring(char *output, int length);
int interval_get_total_time();
int interval_menu_height();