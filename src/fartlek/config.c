#include "pebble.h"

#define STEP_TIME_PKEY 20
#define MAX_TIME_PKEY 21
#define REST_TIME_PKEY 22
#define ROUNDS_PKEY 23
#define EXTENDED_REST_PKEY 24
#define EXTENDED_REST_TIME_PKEY 25
#define EXTENDED_REST_ROUNDS_PKEY 26

#define STEP_TIME_DEFAULT 30
#define MAX_TIME_DEFAULT 120
#define REST_TIME_DEFAULT 45
#define ROUNDS_DEFAULT 3
#define EXTENDED_REST_DEFAULT false
#define EXTENDED_REST_TIME_DEFAULT 60
#define EXTENDED_REST_ROUNDS_DEFAULT 3

int fartlek_step_time = STEP_TIME_DEFAULT;
int fartlek_max_time = MAX_TIME_DEFAULT;
int fartlek_rest_time = REST_TIME_DEFAULT;
int fartlek_rounds = ROUNDS_DEFAULT;
bool fartlek_extended_rest = EXTENDED_REST_DEFAULT;
int fartlek_extended_rest_time = EXTENDED_REST_TIME_DEFAULT;
int fartlek_extended_rest_rounds = EXTENDED_REST_ROUNDS_DEFAULT;

void fartlek_read_persistent() {
	fartlek_step_time = persist_exists(STEP_TIME_PKEY) ? persist_read_int(STEP_TIME_PKEY) : STEP_TIME_DEFAULT;
	fartlek_max_time = persist_exists(MAX_TIME_PKEY) ? persist_read_int(MAX_TIME_PKEY) : MAX_TIME_DEFAULT;
	fartlek_rest_time = persist_exists(REST_TIME_PKEY) ? persist_read_int(REST_TIME_PKEY) : REST_TIME_DEFAULT;
	fartlek_rounds = persist_exists(ROUNDS_PKEY) ? persist_read_int(ROUNDS_PKEY) : ROUNDS_DEFAULT;
	fartlek_extended_rest = persist_exists(EXTENDED_REST_PKEY) ? persist_read_bool(EXTENDED_REST_PKEY) : EXTENDED_REST_DEFAULT;
	fartlek_extended_rest_time = persist_exists(EXTENDED_REST_TIME_PKEY) ? persist_read_int(EXTENDED_REST_TIME_PKEY) : EXTENDED_REST_TIME_DEFAULT;
	fartlek_extended_rest_rounds = persist_exists(EXTENDED_REST_ROUNDS_PKEY) ? persist_read_int(EXTENDED_REST_ROUNDS_PKEY) : EXTENDED_REST_ROUNDS_DEFAULT;
}

void fartlek_write_persistent() {
	persist_write_int(STEP_TIME_PKEY, fartlek_step_time);
	persist_write_int(MAX_TIME_PKEY, fartlek_max_time);
	persist_write_int(REST_TIME_PKEY, fartlek_rest_time);
	persist_write_int(ROUNDS_PKEY, fartlek_rounds);
	persist_write_bool(EXTENDED_REST_PKEY, fartlek_extended_rest);
	persist_write_int(EXTENDED_REST_TIME_PKEY, fartlek_extended_rest_time);
	persist_write_int(EXTENDED_REST_ROUNDS_PKEY, fartlek_extended_rest_rounds);
}