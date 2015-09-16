#include <pebble.h>
#include "config.h"
#include "../common/tools.h"

#define WORKOUT_TIME_DEFAULT 45
#define REST_TIME_DEFAULT 20
#define ROUNDS_DEFAULT 15
#define WARM_UP_DEFAULT 0
#define COOL_DOWN_DEFAULT 0
#define REST_AFTER_LAST_WORKOUT_DEFAULT false

#define EXTENDED_REST_DEFAULT true
#define EXTENDED_REST_TIME_DEFAULT 60
#define EXTENDED_REST_ROUNDS_DEFAULT 5

IntervalSettings interval_settings;

void interval_persist_read(int pkey) {
	int bytesRead = persist_read_data(pkey, &interval_settings, sizeof(IntervalSettings));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "interval:persist:read - pkey: %d, read: %d bytes", pkey, bytesRead);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "interval settings - time: %d, rest: %d", interval_settings.workout_time, interval_settings.rest_time);
}

void interval_persist_write(int pkey) {
	int bytesWritten = persist_write_data(pkey, &interval_settings, sizeof(IntervalSettings));
	int expectedBytesWritten = sizeof(IntervalSettings);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "interval:persist:write - pkey: %d, expected written: %d bytes. actual written: %d bytes", pkey, expectedBytesWritten, bytesWritten);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "interval settings - time: %d, rest: %d", interval_settings.workout_time, interval_settings.rest_time);
}

static void interval_persist_reset(int pkey) {
	interval_settings.workout_time = WORKOUT_TIME_DEFAULT;
	interval_settings.rest_time = REST_TIME_DEFAULT;
	interval_settings.rounds = ROUNDS_DEFAULT;
	interval_settings.warm_up = WARM_UP_DEFAULT;
	interval_settings.cool_down = COOL_DOWN_DEFAULT;
	interval_settings.rest_after_last_workout = REST_AFTER_LAST_WORKOUT_DEFAULT;

	interval_settings.extended_rest.active = EXTENDED_REST_DEFAULT;
	interval_settings.extended_rest.rest_time = EXTENDED_REST_TIME_DEFAULT;
	interval_settings.extended_rest.rounds = EXTENDED_REST_ROUNDS_DEFAULT;

	interval_persist_write(pkey);
}

void interval_persist_migrate(int pkey, int version) {
	switch (version) {
		case 0:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "interval:persist:migrate - unknown version, resetting settings");
			interval_persist_reset(pkey);
			break;
		case 6:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "interval:persist:migrate - v6 to current version");
			#define WORKOUT_TIME_PKEY 1
			#define REST_TIME_PKEY 2
			#define ROUNDS_PKEY 3
			#define EXTENDED_REST_PKEY 4
			#define EXTENDED_REST_TIME_PKEY 5
			#define EXTENDED_REST_ROUNDS_PKEY 6
			#define WARM_UP_PKEY 7
			#define COOL_DOWN_PKEY 8
			#define REST_AFTER_LAST_WORKOUT_PKEY 9
			interval_settings.workout_time = persist_exists(WORKOUT_TIME_PKEY) ? persist_read_int(WORKOUT_TIME_PKEY) : WORKOUT_TIME_DEFAULT;
			interval_settings.rest_time = persist_exists(REST_TIME_PKEY) ? persist_read_int(REST_TIME_PKEY) : REST_TIME_DEFAULT;
			interval_settings.rounds = persist_exists(ROUNDS_PKEY) ? persist_read_int(ROUNDS_PKEY) : ROUNDS_DEFAULT;
			interval_settings.warm_up = persist_exists(WARM_UP_PKEY) ? persist_read_int(WARM_UP_PKEY) : WARM_UP_DEFAULT;
			interval_settings.cool_down = persist_exists(COOL_DOWN_PKEY) ? persist_read_int(COOL_DOWN_PKEY) : COOL_DOWN_DEFAULT;
			interval_settings.rest_after_last_workout = persist_exists(REST_AFTER_LAST_WORKOUT_PKEY) ? persist_read_bool(REST_AFTER_LAST_WORKOUT_PKEY) : REST_AFTER_LAST_WORKOUT_DEFAULT;

			interval_settings.extended_rest.active = persist_exists(EXTENDED_REST_PKEY) ? persist_read_bool(EXTENDED_REST_PKEY) : EXTENDED_REST_DEFAULT;
			interval_settings.extended_rest.rest_time = persist_exists(EXTENDED_REST_TIME_PKEY) ? persist_read_int(EXTENDED_REST_TIME_PKEY) : EXTENDED_REST_TIME_DEFAULT;
			interval_settings.extended_rest.rounds = persist_exists(EXTENDED_REST_ROUNDS_PKEY) ? persist_read_int(EXTENDED_REST_ROUNDS_PKEY) : EXTENDED_REST_ROUNDS_DEFAULT;

			interval_persist_write(pkey);
			break;
		default:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "interval:persist:migrate - already up to date");
			break;
	}
}

char *interval_tostring(char *output, int length) {
	// char total_time_text[7];
	// format_time(total_time_text, interval_get_total_time());

	char workout_time_text[7];
	format_time(workout_time_text, interval_settings.workout_time);
	
	char rest_time_text[7];
	format_time(rest_time_text, interval_settings.rest_time);
	
	if (interval_settings.rounds == 1) {
		if (interval_settings.rest_after_last_workout) {
			snprintf(output, length, "%s+%s", workout_time_text, rest_time_text);
		} else {
			snprintf(output, length, "%s", workout_time_text);
		}
	} else {
		snprintf(output, length, "%s+%s * %d", workout_time_text, rest_time_text, interval_settings.rounds);
	}

	if (interval_settings.extended_rest.active) {
		char extended_rest_time_text[7];
		format_time(extended_rest_time_text, interval_settings.extended_rest.rest_time);
		
		char erbuf[10];
		snprintf(erbuf, sizeof erbuf, " +%s/%d", extended_rest_time_text, interval_settings.extended_rest.rounds);
		strncat(output, erbuf, sizeof erbuf);
	}

	if (interval_settings.warm_up || interval_settings.cool_down) {
		strcat(output, "\n");
		char wcbuf[12];

		if (interval_settings.warm_up) {
			char warm_up_text[7];
			format_time(warm_up_text, interval_settings.warm_up);
			snprintf(wcbuf, sizeof wcbuf, "WU: %s ", warm_up_text);
			strncat(output, wcbuf, sizeof wcbuf);
		}

		if (interval_settings.cool_down) {
			char cool_down_text[7];
			format_time(cool_down_text, interval_settings.cool_down);
			snprintf(wcbuf, sizeof wcbuf, "CD: %s ", cool_down_text);
			strncat(output, wcbuf, sizeof wcbuf);
		}
	}
	
	return output;
}

int interval_menu_height() {
	if (interval_settings.warm_up || interval_settings.cool_down) {
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

	total_time += interval_settings.workout_time;
	total_time += interval_settings.rest_time;
	total_time *= interval_settings.rounds;
	if (!interval_settings.rest_after_last_workout) {
		total_time -= interval_settings.rest_time;
	}

	if (interval_settings.extended_rest.active) {
		if (interval_settings.rounds % interval_settings.extended_rest.rounds == 0) {
			total_time += ((interval_settings.rounds / interval_settings.extended_rest.rounds) - 1) * (interval_settings.extended_rest.rest_time - interval_settings.rest_time);
		} else {
			total_time += (interval_settings.rounds / interval_settings.extended_rest.rounds) * (interval_settings.extended_rest.rest_time - interval_settings.rest_time);
		}
	}

	total_time += interval_settings.warm_up;
	total_time += interval_settings.cool_down;

	return total_time;
}