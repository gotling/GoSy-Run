#include <pebble.h>
#include "config.h"
#include "common/tools.h"

#define STEP_TIME_DEFAULT 30
#define MAX_TIME_DEFAULT 120
#define SLOW_TIME_DEFAULT 45
#define ROUNDS_DEFAULT 3
#define EXTENDED_SLOW_DEFAULT false
#define EXTENDED_SLOW_TIME_DEFAULT 60
#define EXTENDED_SLOW_ROUNDS_DEFAULT 3
#define LADDER_DIRECTION_DEFAULT DESC

LadderSettings ladder_settings;

void ladder_persist_read(int pkey) {
	int bytesRead = persist_read_data(pkey, &ladder_settings, sizeof(LadderSettings));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "ladder:persist:read - pkey: %d, read: %d bytes", pkey, bytesRead);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "ladder settings - step: %d, max: %d", ladder_settings.step_time, ladder_settings.max_time);
}

void ladder_persist_write(int pkey) {
	int bytesWritten = persist_write_data(pkey, &ladder_settings, sizeof(LadderSettings));
	int expectedBytesWritten = sizeof(LadderSettings);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "ladder:persist:write - pkey: %d, expected written: %d bytes. actual written: %d bytes", pkey, expectedBytesWritten, bytesWritten);
	APP_LOG(APP_LOG_LEVEL_DEBUG, "ladder settings - step: %d, max: %d", ladder_settings.step_time, ladder_settings.max_time);
}

static void ladder_persist_reset(int pkey) {
	ladder_settings.step_time = STEP_TIME_DEFAULT;
	ladder_settings.max_time = MAX_TIME_DEFAULT;
	ladder_settings.slow_time = SLOW_TIME_DEFAULT;
	ladder_settings.rounds = ROUNDS_DEFAULT;
	ladder_settings.direction = LADDER_DIRECTION_DEFAULT;

	ladder_settings.extended_slow.active = EXTENDED_SLOW_DEFAULT;
	ladder_settings.extended_slow.slow_time = EXTENDED_SLOW_TIME_DEFAULT;
	ladder_settings.extended_slow.rounds = EXTENDED_SLOW_ROUNDS_DEFAULT;

	ladder_persist_write(pkey);
}

void ladder_persist_migrate(int pkey, int version) {
	switch (version) {
		case 0:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "ladder:persist:migrate - unknown version, resetting settings");
			ladder_persist_reset(pkey);
			break;
		case 6:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "ladder:persist:migrate - v6 to current version");
			#define STEP_TIME_PKEY 20
			#define MAX_TIME_PKEY 21
			#define SLOW_TIME_PKEY 22
			#define ROUNDS_PKEY 23
			#define EXTENDED_SLOW_PKEY 24
			#define EXTENDED_SLOW_TIME_PKEY 25
			#define EXTENDED_SLOW_ROUNDS_PKEY 26
			#define LADDER_DIRECTION_PKEY 27

			ladder_settings.step_time = persist_exists(STEP_TIME_PKEY) ? persist_read_int(STEP_TIME_PKEY) : STEP_TIME_DEFAULT;
			ladder_settings.max_time = persist_exists(MAX_TIME_PKEY) ? persist_read_int(MAX_TIME_PKEY) : MAX_TIME_DEFAULT;
			ladder_settings.slow_time = persist_exists(SLOW_TIME_PKEY) ? persist_read_int(SLOW_TIME_PKEY) : SLOW_TIME_DEFAULT;
			ladder_settings.rounds = persist_exists(ROUNDS_PKEY) ? persist_read_int(ROUNDS_PKEY) : ROUNDS_DEFAULT;
			ladder_settings.direction = persist_exists(LADDER_DIRECTION_PKEY) ? persist_read_int(LADDER_DIRECTION_PKEY) : LADDER_DIRECTION_DEFAULT;

			ladder_settings.extended_slow.active = persist_exists(EXTENDED_SLOW_PKEY) ? persist_read_bool(EXTENDED_SLOW_PKEY) : EXTENDED_SLOW_DEFAULT;
			ladder_settings.extended_slow.slow_time = persist_exists(EXTENDED_SLOW_TIME_PKEY) ? persist_read_int(EXTENDED_SLOW_TIME_PKEY) : EXTENDED_SLOW_TIME_DEFAULT;
			ladder_settings.extended_slow.rounds = persist_exists(EXTENDED_SLOW_ROUNDS_PKEY) ? persist_read_int(EXTENDED_SLOW_ROUNDS_PKEY) : EXTENDED_SLOW_ROUNDS_DEFAULT;

			ladder_persist_write(pkey);
			break;
		default:
			APP_LOG(APP_LOG_LEVEL_DEBUG, "ladder:persist:migrate - already up to date");
			break;
	}
}

char* ladder_direction_to_string(char *buf, int direction) {
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
		case DESC_ASC:
			strcpy(buf, "Desc-Asc");
			break;
	}

	return buf;
}

int ladder_get_step_count() {
	switch (ladder_settings.direction) {
		case ASC_DESC:
		case DESC_ASC:
			return (ladder_settings.max_time / ladder_settings.step_time) * 2 - 1;
		default:
			return ladder_settings.max_time / ladder_settings.step_time;
	}
}

int *ladder_set_up(int *round_time) {
	int rounds = ladder_settings.max_time / ladder_settings.step_time;

	switch (ladder_settings.direction) {
		case ASC:
			for (int i = 0; i < rounds; ++i) {
				round_time[i] =  ((i + 1) * ladder_settings.step_time);
			}
			break;
		case ASC_DESC:
			for (int i = 0; i < rounds; ++i) {
				round_time[i] =  ((i + 1) * ladder_settings.step_time);
			}
			for (int i = rounds; i < (rounds * 2) - 1; ++i) {
				round_time[i] = round_time[i - 1] - ladder_settings.step_time;
			}
			break;
		case DESC:
			for (int i = 0; i < rounds; ++i) {
				round_time[i] = ladder_settings.max_time - (i * ladder_settings.step_time);
			}
			break;
		case DESC_ASC:
			for (int i = 0; i < rounds; ++i) {
				round_time[i] = ladder_settings.max_time - (i * ladder_settings.step_time);
			}
			for (int i = rounds; i < (rounds * 2) - 1; ++i) {
				round_time[i] =  round_time[i - 1] + ladder_settings.step_time;
			}
			break;
	}

	return round_time;
}


char *ladder_tostring(char *output, int length) {
	strcpy(output, "");

	char fatlek_text[20];
	int rounds = ladder_get_step_count();
	int round_time[rounds];
	ladder_set_up(round_time);

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
		switch(ladder_settings.direction) {
			case ASC:
			case DESC:
				snprintf(fatlek_text, sizeof fatlek_text, "%d-%d..%d", round_time[0], round_time[1], round_time[rounds-1]);
				break;
			case ASC_DESC:
			case DESC_ASC:
				if (rounds % 2 == 0) {
					snprintf(fatlek_text, sizeof fatlek_text, "%d..%d-%d..%d", round_time[0], round_time[rounds / 2 - 1], round_time[rounds / 2], round_time[rounds-1]);
				} else {
					snprintf(fatlek_text, sizeof fatlek_text, "%d-%d..%d..%d", round_time[0], round_time[1], round_time[rounds / 2], round_time[rounds-1]);
				}
				break;
		}

		strncat(output, fatlek_text, sizeof fatlek_text);
	}

	snprintf(fatlek_text, sizeof fatlek_text, " * %d", ladder_settings.rounds);
	strncat(output, fatlek_text, sizeof fatlek_text);
	
	return output;
}

int ladder_get_total_time() {
	int total_time = 0;
	int rounds = ladder_get_step_count();
	int round_time[rounds];
	ladder_set_up(round_time);

	for (int i = 0; i < rounds; ++i) {
		total_time += round_time[i];
		total_time += ladder_settings.slow_time;
	}

	total_time *= ladder_settings.rounds;
	total_time -= ladder_settings.slow_time;

	return total_time;
}