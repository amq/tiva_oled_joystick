#include "Common.h"
#include "OledTask.h"
#include "JoystickTask.h"

int verbose = 0;

int main(void) {
  Board_initGeneral();
  Board_initGPIO();
  Board_initI2C();
  Board_initSPI();

  Mailbox_Handle mailboxHandle;
  Mailbox_Params mailboxParams;
  Mailbox_Params_init(&mailboxParams);

  mailboxHandle = Mailbox_create(sizeof(uint8_t) * 2, 1, &mailboxParams, NULL);

  if (mailboxHandle == NULL) {
    System_abort("Mailbox create failed\n");
  }

  OledTask_init(mailboxHandle);
  JoystickTask_init(mailboxHandle);

  BIOS_start();

  return 0;
}
