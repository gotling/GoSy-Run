#pragma once

extern int interval_workout_time;
extern int interval_rest_time;
extern int interval_rounds;
extern bool interval_extended_rest;
extern int interval_extended_rest_time;
extern int interval_extended_rest_rounds;

void interval_read_persistent();
void interval_write_persistent();