#ifndef UTIL_H
#define UTIL_H

#include <stdlib.h>

#include <iostream>

#define PANIC(s)                 \
  {                              \
    std::cerr << s << std::endl; \
    exit(EXIT_FAILURE);          \
  }

#endif
