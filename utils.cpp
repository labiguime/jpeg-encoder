#include "utils.h"

int clamp(int num, int min, int max) {
   return num <= min ? min : num >= max ? max : num;
}
