#include <pebble.h>
#include "config.h"
#include "../common/tools.h"

#define STEP_TIME_PKEY 20
#define MAX_TIME_PKEY 21
#define SLOW_TIME_PKEY 22
#define ROUNDS_PKEY 23
#define EXTENDED_SLOW_PKEY 24
#define EXTENDED_SLOW_TIME_PKEY 25
#define EXTENDED_SLOW_ROUNDS_PKEY 26
#define LADDER_DIRECTION_PKEY 27

#define STEP_TIME_DEFAULT 30
#define MAX_TIME_DEFAULT 120
#define REST_TIME_DEFAULT 45
#define ROUNDS_DEFAULT 3
#define EXTENDED_SLOW_DEFAULT false
#define EXTENDED_SLOW_TIME_DEFAULT 60
#define EXTENDED_SLOW_ROUNDS_DEFAULT 3
#define LADDER_DIRECTION_DEFAULT DESC

int ladder_step_time = STEP_TIME_DEFAULT;
int ladder_max_time = MAX_TIME_DEFAULT;
int ladder_slow_time = REST_TIME_DEFAULT;
int ladder_rounds = ROUNDS_DEFAULT;
bool ladder_extended_slow = EXTENDED_SLOW_DEFAULT;
int ladder_extended_slow_time = EXTENDED_SLOW_TIME_DEFAULT;
int ladder_extended_slow_rounds = EXTENDED_SLOW_ROUNDS_DEFAULT;
LadderDirection ladder_direction = LADDER_DIRECTION_DEFAULT;

void ladder_read_persistent() {
	ladder_step_time = persist_exists(STEP_TIME_PKEY) ? persist_read_int(STEP_TIME_PKEY) : STEP_TIME_DEFAULT;
	ladder_max_time = persist_exists(MAX_TIME_PKEY) ? persist_read_int(MAX_TIME_PKEY) : MAX_TIME_DEFAULT;
	ladder_slow_time = persist_exists(SLOW_TIME_PKEY) ? persist_read_int(SLOW_TIME_PKEY) : REST_TIME_DEFAULT;
	ladder_rounds = persist_exists(ROUNDS_PKEY) ? persist_read_int(ROUNDS_PKEY) : ROUNDS_DEFAULT;
	ladder_extended_slow = persist_exists(EXTENDED_SLOW_PKEY) ? persist_read_bool(EXTENDED_SLOW_PKEY) : EXTENDED_SLOW_DEFAULT;
	ladder_extended_slow_time = persist_exists(EXTENDED_SLOW_TIME_PKEY) ? persist_read_int(EXTENDED_SLOW_TIME_PKEY) : EXTENDED_SLOW_TIME_DEFAULT;
	ladder_extended_slow_rounds = persist_exists(EXTENDED_SLOW_ROUNDS_PKEY) ? persist_read_int(EXTENDED_SLOW_ROUNDS_PKEY) : EXTENDED_SLOW_ROUNDS_DEFAULT;
	ladder_direction = persist_exists(LADDER_DIRECTION_PKEY) ? persist_read_int(LADDER_DIRECTION_PKEY) : LADDER_DIRECTION_DEFAULT;
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Ladder Read. Step %d, Max %d", ladder_step_time, ladder_max_time);
}

void ladder_write_persistent() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "Ladder Write. Step %d, Max %d", ladder_step_time, ladder_max_time);
	persist_write_int(STEP_TIME_PKEY, ladder_step_time);
	persist_write_int(MAX_TIME_PKEY, ladder_max_time);
	persist_write_int(SLOW_TIME_PKEY, ladder_slow_time);
	persist_write_int(ROUNDS_PKEY, ladder_rounds);
	persist_write_bool(EXTENDED_SLOW_PKEY, ladder_extended_slow);
	persist_write_int(EXTENDED_SLOW_TIME_PKEY, ladder_extended_slow_time);
	persist_write_int(EXTENDED_SLOW_ROUNDS_PKEY, ladder_extended_slow_rounds);
	persist_write_int(LADDER_DIRECTION_PKEY, ladder_direction);
}

char* lookup_direction(char *buf, int direction) {
	switch(direction) {
		case ASC:
			strcpy(buf, "Ascending");
			break;
		case DESC:
			strcpy(buf, "Descending");
			break;
		case ASC_DESC:
			strcpy(buf, "Asc-Desc");
			break;
	}

	return buf;
}

int ladder_get_step_count() {
	switch (ladder_direction) {
		case ASC_DESC:
			return (ladder_max_time / ladder_step_time) * 2 - 1;
		default:
			return ladder_max_time / ladder_step_time;
	}
}

int *set_up_ladder(int *round_time) {
	int rounds = ladder_max_time / ladder_step_time;

	switch (ladder_direction) {
		case ASC:
			for (int i = 0; i < rounds; ++i) {
				round_time[i] =  ((i + 1) * ladder_step_time);
			}
			break;
		case ASC_DESC:
			for (int i = 0; i < rounds; ++i) {
				round_time[i] =  ((i + 1) * ladder_step_time);
			}
			for (int i = rounds; i < (rounds * 2) - 1; ++i) {
				round_time[i] = ladder_max_time - ((i - (rounds - 1)) * ladder_step_time);
			}
			break;
		case DESC:
			for (int i = 0; i < rounds; ++i) {
				round_time[i] = ladder_max_time - (i * ladder_step_time);
			}
			break;
	}

	return round_time;
}


char *ladder_tostring(char *output, int length) {
	// Line 1
	// char total_time_text[7];
	// format_time(total_time_text, ladder_get_total_time());
	strcpy(output, "");
	// snprintf(output, length, "Total time: %s\n", total_time_text);

	// Line 2
	char fatlek_text[20];
	int rounds = ladder_get_step_count();
	int round_time[rounds];
	set_up_ladder(round_time);

	if (rounds <= 5) {
		for (int i = 0; i < rounds; ++i) {
			if(i < (rounds - 1)) {
				snprintf(fatlek_text, sizeof fatlek_text, "%d-", round_time[i]);
			} else {
				snprintf(fatlek_text, sizeof fatlek_text, "%d", round_time[i]);
			}
			strncat(output, fatlek_text, sizeof fatlek_text);
		}
	} else {
		switch(ladder_direction) {
			case ASC:
			case DESC:
				snprintf(fatlek_text, sizeof fatlek_text, "%d-%d..%d", round_time[0], round_time[1], round_time[rounds-1]);
				break;
			case ASC_DESC:
				if (rounds % 2 == 0) {
					snprintf(fatlek_text, sizeof fatlek_text, "%d..%d-%d..%d", round_time[0], round_time[rounds / 2 - 1], round_time[rounds / 2], round_time[rounds-1]);
				} else {
					snprintf(fatlek_text, sizeof fatlek_text, "%d-%d..%d..%d", round_time[0], round_time[1], round_time[rounds / 2], round_time[rounds-1]);
				}
				break;
		}

		strncat(output, fatlek_text, sizeof fatlek_text);
	}

	snprintf(fatlek_text, sizeof fatlek_text, " * %d", ladder_rounds);
	strncat(output, fatlek_text, sizeof fatlek_text);
	return output;
}

int ladder_get_total_time() {
	int total_time = 0;
	int rounds = ladder_get_step_count();
	int round_time[rounds];
	set_up_ladder(round_time);

	for (int i = 0; i < rounds; ++i) {
		total_time += round_time[i];
		total_time += ladder_slow_time;
	}

	total_time *= ladder_rounds;
	total_time -= ladder_slow_time;

	return total_time;
}