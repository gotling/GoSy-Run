#include <pebble.h>
#include "interval_config.h"
#include "../common/tools.h"

#define WORKOUT_TIME_PKEY 1
#define REST_TIME_PKEY 2
#define ROUNDS_PKEY 3
#define EXTENDED_REST_PKEY 4
#define EXTENDED_REST_TIME_PKEY 5
#define EXTENDED_REST_ROUNDS_PKEY 6
#define WARM_UP_PKEY 7
#define COOL_DOWN_PKEY 8

#define WORKOUT_TIME_DEFAULT 45
#define REST_TIME_DEFAULT 20
#define ROUNDS_DEFAULT 15
#define EXTENDED_REST_DEFAULT true
#define EXTENDED_REST_TIME_DEFAULT 60
#define EXTENDED_REST_ROUNDS_DEFAULT 5
#define WARM_UP_DEFAULT 0
#define COOL_DOWN_DEFAULT 0

int interval_workout_time = WORKOUT_TIME_DEFAULT;
int interval_rest_time = REST_TIME_DEFAULT;
int interval_rounds = ROUNDS_DEFAULT;
bool interval_extended_rest = EXTENDED_REST_DEFAULT;
int interval_extended_rest_time = EXTENDED_REST_TIME_DEFAULT;
int interval_extended_rest_rounds = EXTENDED_REST_ROUNDS_DEFAULT;
int interval_warm_up = WARM_UP_DEFAULT;
int interval_cool_down = COOL_DOWN_DEFAULT;

void interval_read_persistent() {
	interval_workout_time = persist_exists(WORKOUT_TIME_PKEY) ? persist_read_int(WORKOUT_TIME_PKEY) : WORKOUT_TIME_DEFAULT;
	interval_rest_time = persist_exists(REST_TIME_PKEY) ? persist_read_int(REST_TIME_PKEY) : REST_TIME_DEFAULT;
	interval_rounds = persist_exists(ROUNDS_PKEY) ? persist_read_int(ROUNDS_PKEY) : ROUNDS_DEFAULT;
	interval_extended_rest = persist_exists(EXTENDED_REST_PKEY) ? persist_read_bool(EXTENDED_REST_PKEY) : EXTENDED_REST_DEFAULT;
	interval_extended_rest_time = persist_exists(EXTENDED_REST_TIME_PKEY) ? persist_read_int(EXTENDED_REST_TIME_PKEY) : EXTENDED_REST_TIME_DEFAULT;
	interval_extended_rest_rounds = persist_exists(EXTENDED_REST_ROUNDS_PKEY) ? persist_read_int(EXTENDED_REST_ROUNDS_PKEY) : EXTENDED_REST_ROUNDS_DEFAULT;
	interval_warm_up = persist_exists(WARM_UP_PKEY) ? persist_read_int(WARM_UP_PKEY) : WARM_UP_DEFAULT;
	interval_cool_down = persist_exists(COOL_DOWN_PKEY) ? persist_read_int(COOL_DOWN_PKEY) : COOL_DOWN_DEFAULT;
}

void interval_write_persistent() {
	persist_write_int(WORKOUT_TIME_PKEY, interval_workout_time);
	persist_write_int(REST_TIME_PKEY, interval_rest_time);
	persist_write_int(ROUNDS_PKEY, interval_rounds);
	persist_write_bool(EXTENDED_REST_PKEY, interval_extended_rest);
	persist_write_int(EXTENDED_REST_TIME_PKEY, interval_extended_rest_time);
	persist_write_int(EXTENDED_REST_ROUNDS_PKEY, interval_extended_rest_rounds);
	persist_write_int(WARM_UP_PKEY, interval_warm_up);
	persist_write_int(COOL_DOWN_PKEY, interval_cool_down);
}

char *interval_tostring(char *output, int length) {
	// char total_time_text[7];
	// format_time(total_time_text, interval_get_total_time());

	char workout_time_text[7];
	format_time(workout_time_text, interval_workout_time);
	
	char rest_time_text[7];
	format_time(rest_time_text, interval_rest_time);
	
	snprintf(output, length, "%s+%s * %d", workout_time_text, rest_time_text, interval_rounds);
	
	if (interval_extended_rest) {
		char extended_rest_time_text[7];
		format_time(extended_rest_time_text, interval_extended_rest_time);
		
		char erbuf[10];
		snprintf(erbuf, sizeof erbuf, " +%s/%d", extended_rest_time_text, interval_extended_rest_rounds);
		strncat(output, erbuf, sizeof erbuf);
	}

	if (interval_warm_up || interval_cool_down) {
		strcat(output, "\n");
		char wcbuf[12];

		if (interval_warm_up) {
			char warm_up_text[7];
			format_time(warm_up_text, interval_warm_up);
			snprintf(wcbuf, sizeof wcbuf, "WU: %s ", warm_up_text);
			strncat(output, wcbuf, sizeof wcbuf);
		}

		if (interval_cool_down) {
			char cool_down_text[7];
			format_time(cool_down_text, interval_cool_down);
			snprintf(wcbuf, sizeof wcbuf, "CD: %s ", cool_down_text);
			strncat(output, wcbuf, sizeof wcbuf);
		}
	}
	
	return output;
}

int interval_menu_height() {
	if (interval_warm_up || interval_cool_down) {
		return MENU_CELL_BASIC_MULTILINE_HEIGHT;
	} else {
		return MENU_CELL_BASIC_HEIGHT;
	}
}
/*
Example
workout 				60
rest 					30
rounds 					10
extended rest ronunds 	5
extended rest time 		60
--------------------------
						900
						870
						900
*/


int interval_get_total_time() {
	int total_time = 0;

	total_time += interval_workout_time;
	total_time += interval_rest_time;
	total_time *= interval_rounds;
	total_time -= interval_rest_time;

	if (interval_extended_rest) {
		if (interval_rounds % interval_extended_rest_rounds == 0) {
			total_time += ((interval_rounds / interval_extended_rest_rounds) - 1) * (interval_extended_rest_time - interval_rest_time);
		} else {
			total_time += (interval_rounds / interval_extended_rest_rounds) * (interval_extended_rest_time - interval_rest_time);
		}
	}

	total_time += interval_warm_up;
	total_time += interval_cool_down;

	return total_time;
}