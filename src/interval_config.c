#include "pebble.h"

#define WORKOUT_TIME_PKEY 1
#define REST_TIME_PKEY 2
#define ROUNDS_PKEY 3
#define EXTENDED_REST_PKEY 4
#define EXTENDED_REST_TIME_PKEY 5
#define EXTENDED_REST_ROUNDS_PKEY 6

#define WORKOUT_TIME_DEFAULT 45
#define REST_TIME_DEFAULT 20
#define ROUNDS_DEFAULT 15
#define EXTENDED_REST_DEFAULT false
#define EXTENDED_REST_TIME_DEFAULT 60
#define EXTENDED_REST_ROUNDS_DEFAULT 5

int interval_workout_time = WORKOUT_TIME_DEFAULT;
int interval_rest_time = REST_TIME_DEFAULT;
int interval_rounds = ROUNDS_DEFAULT;
bool interval_extended_rest = EXTENDED_REST_DEFAULT;
int interval_extended_rest_time = EXTENDED_REST_TIME_DEFAULT;
int interval_extended_rest_rounds = EXTENDED_REST_ROUNDS_DEFAULT;

void interval_read_persistent() {
	interval_workout_time = persist_exists(WORKOUT_TIME_PKEY) ? persist_read_int(WORKOUT_TIME_PKEY) : WORKOUT_TIME_DEFAULT;
	interval_rest_time = persist_exists(REST_TIME_PKEY) ? persist_read_int(REST_TIME_PKEY) : REST_TIME_DEFAULT;
	interval_rounds = persist_exists(ROUNDS_PKEY) ? persist_read_int(ROUNDS_PKEY) : ROUNDS_DEFAULT;
	interval_extended_rest = persist_exists(EXTENDED_REST_PKEY) ? persist_read_bool(EXTENDED_REST_PKEY) : EXTENDED_REST_DEFAULT;
	interval_extended_rest_time = persist_exists(EXTENDED_REST_TIME_PKEY) ? persist_read_int(EXTENDED_REST_TIME_PKEY) : EXTENDED_REST_TIME_DEFAULT;
	interval_extended_rest_rounds = persist_exists(EXTENDED_REST_ROUNDS_PKEY) ? persist_read_int(EXTENDED_REST_ROUNDS_PKEY) : EXTENDED_REST_ROUNDS_DEFAULT;
}

void interval_write_persistent() {
	persist_write_int(WORKOUT_TIME_PKEY, interval_workout_time);
	persist_write_int(REST_TIME_PKEY, interval_rest_time);
	persist_write_int(ROUNDS_PKEY, interval_rounds);
	persist_write_bool(EXTENDED_REST_PKEY, interval_extended_rest);
	persist_write_int(EXTENDED_REST_TIME_PKEY, interval_extended_rest_time);
	persist_write_int(EXTENDED_REST_ROUNDS_PKEY, interval_extended_rest_rounds);
}