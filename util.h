#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>

#include <iostream>

#define STATE_NORMAL 0
#define STATE_EXIT 1
#define STATE_WON_GAME 2

#define WIN_WIDTH 1800
#define WIN_HEIGHT 600

#define LETTER_FIRST 'A'
#define LETTER_LAST 'Z'
#define ABC_LEN (LETTER_LAST - LETTER_FIRST + 1)
#define FONT_PADDING 64

static const char *names[] = {
    "Antelope", "Bumble bee",   "Cat",     "Dog",     "Elephant", "Fox",
    "Gorilla",  "Hippopotamus", "Iguana",  "Jaguar",  "Koala",    "Lemur",
    "Meerkat",  "Nautilus",     "Ostrich", "Panther", "Quokka",   "Rabbit",
    "Snake",    "Tapir",        "Urchin",  "Vulture", "Wolf",     "Xerus",
    "Yak",      "Zebra"};

#define PANIC(s)                 \
  {                              \
    std::cerr << s << std::endl; \
    exit(EXIT_FAILURE);          \
  }

char rand_char();

#endif
