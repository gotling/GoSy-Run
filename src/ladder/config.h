#pragma once

extern int ladder_step_time;
extern int ladder_max_time;
extern int ladder_slow_time;
extern int ladder_rounds;
extern bool ladder_extended_slow;
extern int ladder_extended_slow_time;
extern int ladder_extended_slow_rounds;

char *ladder_tostring(char *output, int length);
int ladder_get_total_time();
void ladder_read_persistent();
void ladder_write_persistent();