#pragma once

typedef enum {
	ASC = 1,
	ASC_DESC,
	DESC,
	DESC_ASC
} LadderDirection;

typedef struct {
	int active;
	int slow_time;
	int rounds;
} ExtendedSlow;

typedef struct {
	int step_time;
	int max_time;
	int slow_time;
	int rounds;
	LadderDirection direction;
	ExtendedSlow extended_slow;
} LadderSettings;

extern LadderSettings ladder_settings;

void ladder_persist_read(int pkey);
void ladder_persist_write(int pkey);
void ladder_persist_migrate(int pkey, int version);

int ladder_get_step_count();
int *ladder_set_up(int *round_time);
char *ladder_tostring(char *output, int length);
char* ladder_direction_to_string(char *buf, int direction);
int ladder_get_total_time();
