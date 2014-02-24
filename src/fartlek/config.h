#pragma once

extern int fartlek_step_time;
extern int fartlek_max_time;
extern int fartlek_slow_time;
extern int fartlek_rounds;
extern bool fartlek_extended_slow;
extern int fartlek_extended_slow_time;
extern int fartlek_extended_slow_rounds;

char *fartlek_tostring(char *output, int length);
int fartlek_get_total_time();
void fartlek_read_persistent();
void fartlek_write_persistent();