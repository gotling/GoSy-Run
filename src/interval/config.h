#pragma once

typedef struct {
	int active;
	int rest_time;
	int rounds;
} ExtendedRest;

typedef struct {
	int workout_time;
	int rest_time;
	int rounds;
	ExtendedRest extended_rest;
	int warm_up;
	int cool_down;
	bool rest_after_last_workout;
} IntervalSettings;

extern IntervalSettings interval_settings;

void interval_persist_read(int pkey);
void interval_persist_write(int pkey);
void interval_persist_migrate(int pkey, int version);

char *interval_tostring(char *output, int length);
int interval_get_total_time();
int interval_menu_height();