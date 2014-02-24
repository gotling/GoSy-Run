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

#define STEP_TIME_DEFAULT 30
#define MAX_TIME_DEFAULT 120
#define REST_TIME_DEFAULT 45
#define ROUNDS_DEFAULT 3
#define EXTENDED_SLOW_DEFAULT false
#define EXTENDED_SLOW_TIME_DEFAULT 60
#define EXTENDED_SLOW_ROUNDS_DEFAULT 3

int ladder_step_time = STEP_TIME_DEFAULT;
int ladder_max_time = MAX_TIME_DEFAULT;
int ladder_slow_time = REST_TIME_DEFAULT;
int ladder_rounds = ROUNDS_DEFAULT;
bool ladder_extended_slow = EXTENDED_SLOW_DEFAULT;
int ladder_extended_slow_time = EXTENDED_SLOW_TIME_DEFAULT;
int ladder_extended_slow_rounds = EXTENDED_SLOW_ROUNDS_DEFAULT;

void ladder_read_persistent() {
	ladder_step_time = persist_exists(STEP_TIME_PKEY) ? persist_read_int(STEP_TIME_PKEY) : STEP_TIME_DEFAULT;
	ladder_max_time = persist_exists(MAX_TIME_PKEY) ? persist_read_int(MAX_TIME_PKEY) : MAX_TIME_DEFAULT;
	ladder_slow_time = persist_exists(SLOW_TIME_PKEY) ? persist_read_int(SLOW_TIME_PKEY) : REST_TIME_DEFAULT;
	ladder_rounds = persist_exists(ROUNDS_PKEY) ? persist_read_int(ROUNDS_PKEY) : ROUNDS_DEFAULT;
	ladder_extended_slow = persist_exists(EXTENDED_SLOW_PKEY) ? persist_read_bool(EXTENDED_SLOW_PKEY) : EXTENDED_SLOW_DEFAULT;
	ladder_extended_slow_time = persist_exists(EXTENDED_SLOW_TIME_PKEY) ? persist_read_int(EXTENDED_SLOW_TIME_PKEY) : EXTENDED_SLOW_TIME_DEFAULT;
	ladder_extended_slow_rounds = persist_exists(EXTENDED_SLOW_ROUNDS_PKEY) ? persist_read_int(EXTENDED_SLOW_ROUNDS_PKEY) : EXTENDED_SLOW_ROUNDS_DEFAULT;
}

void ladder_write_persistent() {
	persist_write_int(STEP_TIME_PKEY, ladder_step_time);
	persist_write_int(MAX_TIME_PKEY, ladder_max_time);
	persist_write_int(SLOW_TIME_PKEY, ladder_slow_time);
	persist_write_int(ROUNDS_PKEY, ladder_rounds);
	persist_write_bool(EXTENDED_SLOW_PKEY, ladder_extended_slow);
	persist_write_int(EXTENDED_SLOW_TIME_PKEY, ladder_extended_slow_time);
	persist_write_int(EXTENDED_SLOW_ROUNDS_PKEY, ladder_extended_slow_rounds);
}


char *ladder_tostring(char *output, int length) {
	// Line 1
	char total_time_text[7];
	format_time(total_time_text, ladder_get_total_time());
	snprintf(output, length, "Total time: %s\n", total_time_text);

	// Line 2
	char fatlek_text[10];
	int rounds = ladder_max_time / ladder_step_time;
	int round_time[rounds];

	for (int i = 0; i < rounds; ++i) {
		round_time[i] = ladder_max_time - (i * ladder_step_time);
		if(i < (rounds - 1)) {
			snprintf(fatlek_text, sizeof fatlek_text, "%d-", round_time[i]);
		} else {
			snprintf(fatlek_text, sizeof fatlek_text, "%d", round_time[i]);
		}
		strncat(output, fatlek_text, sizeof fatlek_text);
	}

	snprintf(fatlek_text, sizeof fatlek_text, " * %d", ladder_rounds);
	strncat(output, fatlek_text, sizeof fatlek_text);
		
	return output;
}

int ladder_get_total_time() {
	int total_time = 0;
	int rounds = ladder_max_time / ladder_step_time;
	for (int i = 0; i < rounds; ++i) {
		total_time += (ladder_max_time - (i * ladder_step_time));
		total_time += ladder_slow_time;
	}
	total_time *= ladder_rounds;
	total_time -= ladder_slow_time;

	return total_time;
}