#include <stdlib.h>
#include <time.h>

#include "app.h"
#include "image.h"
#include "sound.h"
#include "util.h"

int main() {
  srand(time(NULL));

  App app;
  app.init();
  app.run();

  exit(EXIT_SUCCESS);
}
