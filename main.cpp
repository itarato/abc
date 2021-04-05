#include <stdlib.h>
#include <time.h>

#include "app.h"

int main() {
  srand(time(NULL));

  App app;
  app.init();
  app.run();

  exit(EXIT_SUCCESS);
}
