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

int fartlek_step_time = STEP_TIME_DEFAULT;
int fartlek_max_time = MAX_TIME_DEFAULT;
int fartlek_slow_time = REST_TIME_DEFAULT;
int fartlek_rounds = ROUNDS_DEFAULT;
bool fartlek_extended_slow = EXTENDED_SLOW_DEFAULT;
int fartlek_extended_slow_time = EXTENDED_SLOW_TIME_DEFAULT;
int fartlek_extended_slow_rounds = EXTENDED_SLOW_ROUNDS_DEFAULT;

void fartlek_read_persistent() {
	fartlek_step_time = persist_exists(STEP_TIME_PKEY) ? persist_read_int(STEP_TIME_PKEY) : STEP_TIME_DEFAULT;
	fartlek_max_time = persist_exists(MAX_TIME_PKEY) ? persist_read_int(MAX_TIME_PKEY) : MAX_TIME_DEFAULT;
	fartlek_slow_time = persist_exists(SLOW_TIME_PKEY) ? persist_read_int(SLOW_TIME_PKEY) : REST_TIME_DEFAULT;
	fartlek_rounds = persist_exists(ROUNDS_PKEY) ? persist_read_int(ROUNDS_PKEY) : ROUNDS_DEFAULT;
	fartlek_extended_slow = persist_exists(EXTENDED_SLOW_PKEY) ? persist_read_bool(EXTENDED_SLOW_PKEY) : EXTENDED_SLOW_DEFAULT;
	fartlek_extended_slow_time = persist_exists(EXTENDED_SLOW_TIME_PKEY) ? persist_read_int(EXTENDED_SLOW_TIME_PKEY) : EXTENDED_SLOW_TIME_DEFAULT;
	fartlek_extended_slow_rounds = persist_exists(EXTENDED_SLOW_ROUNDS_PKEY) ? persist_read_int(EXTENDED_SLOW_ROUNDS_PKEY) : EXTENDED_SLOW_ROUNDS_DEFAULT;
}

void fartlek_write_persistent() {
	persist_write_int(STEP_TIME_PKEY, fartlek_step_time);
	persist_write_int(MAX_TIME_PKEY, fartlek_max_time);
	persist_write_int(SLOW_TIME_PKEY, fartlek_slow_time);
	persist_write_int(ROUNDS_PKEY, fartlek_rounds);
	persist_write_bool(EXTENDED_SLOW_PKEY, fartlek_extended_slow);
	persist_write_int(EXTENDED_SLOW_TIME_PKEY, fartlek_extended_slow_time);
	persist_write_int(EXTENDED_SLOW_ROUNDS_PKEY, fartlek_extended_slow_rounds);
}


char *fartlek_tostring(char *output, int length) {
	// Line 1
	char total_time_text[7];
	format_time(total_time_text, fartlek_get_total_time());
	snprintf(output, length, "Total time: %s\n", total_time_text);

	// Line 2
	char fatlek_text[10];
	int rounds = fartlek_max_time / fartlek_step_time;
	int round_time[rounds];

	for (int i = 0; i < rounds; ++i) {
		round_time[i] = fartlek_max_time - (i * fartlek_step_time);
		if(i < (rounds - 1)) {
			snprintf(fatlek_text, sizeof fatlek_text, "%d-", round_time[i]);
		} else {
			snprintf(fatlek_text, sizeof fatlek_text, "%d", round_time[i]);
		}
		strncat(output, fatlek_text, sizeof fatlek_text);
	}

	snprintf(fatlek_text, sizeof fatlek_text, " * %d", fartlek_rounds);
	strncat(output, fatlek_text, sizeof fatlek_text);
		
	return output;
}

int fartlek_get_total_time() {
	int total_time = 0;
	int rounds = fartlek_max_time / fartlek_step_time;
	for (int i = 0; i < rounds; ++i) {
		total_time += (fartlek_max_time - (i * fartlek_step_time));
		total_time += fartlek_slow_time;
	}
	total_time *= fartlek_rounds;
	total_time -= fartlek_slow_time;

	return total_time;
}