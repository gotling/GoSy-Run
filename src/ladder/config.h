#pragma once

typedef enum {
	ASC = 1,
	ASC_DESC,
	DESC
} LadderDirection;

extern int ladder_step_time;
extern int ladder_max_time;
extern int ladder_slow_time;
extern int ladder_rounds;
extern bool ladder_extended_slow;
extern int ladder_extended_slow_time;
extern int ladder_extended_slow_rounds;
extern LadderDirection ladder_direction;

void ladder_read_persistent();
void ladder_write_persistent();

int ladder_get_step_count();
int *set_up_ladder(int *round_time);
char *ladder_tostring(char *output, int length);
char* lookup_direction(char *buf, int direction);
int ladder_get_total_time();
