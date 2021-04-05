#include "util.h"

char rand_char() {
  return LETTER_FIRST + rand() % (LETTER_LAST - LETTER_FIRST + 1);
}
