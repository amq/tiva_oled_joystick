#include "Common.h"
#include "LedTask.h"
#include "OledTask.h"

int main(void) {
  Board_initGeneral();
  Board_initGPIO();
  Board_initI2C();
  Board_initSPI();

  LedTask_init();
  OledTask_init();

  BIOS_start();

  return 0;
}
