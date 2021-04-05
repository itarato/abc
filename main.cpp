#include <stdlib.h>
#include <time.h>

#include "app.h"
#include "image.h"
#include "sound.h"
#include "util.h"

App app;

void cleanup() { app.cleanup(); }

int main() {
  srand(time(NULL));

  app.init();
  atexit(cleanup);

  app.run();

  exit(EXIT_SUCCESS);
}
