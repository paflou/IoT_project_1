#include <math.h>

#ifndef UTILS_H
#define UTILS_H

struct data_form {
  unsigned ID;
  unsigned count;
  double temp;
  double hum;
};

void double2str(char *buf, double num) {
  int integer_part = (int)num;
  int fractional_part = (int)((num - integer_part) * 100);

  sprintf(buf, "%d.%d", integer_part, fractional_part);
}

double temperature_int2double(int value) {
  return value != -1 ? (0.01*value) - 39.60 : -1;
}

double humidity_int2double(int value) {
  return value != -1 ? ((0.0405*value) - 4) + ((-2.8 * 0.000001)*(pow(value,2))) : -1;
}

#endif