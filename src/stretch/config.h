#pragma once

typedef struct {
	bool program;
	int time;
	int prepare;
} StretchSettings;

extern StretchSettings stretch_settings;

void stretch_persist_read(int pkey);
void stretch_persist_write(int pkey);
void stretch_persist_migrate(int pkey, int version);

char *stretch_tostring(char *output, int length);
int stretch_get_total_time();