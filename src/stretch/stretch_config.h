#pragma once

extern int stretch_stretch_time;
extern int stretch_prepare_time;

void stretch_read_persistent();
void stretch_write_persistent();

char *stretch_tostring(char *output, int length);