#include "Common.h"
#include "JoystickTask.h"
#include "OledTask.h"

int main(void) {
  Board_initGeneral(120*1000*1000);
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

  JoystickTask_init(mailboxHandle);
  OledTask_init(mailboxHandle);

  BIOS_start();

  return 0;
}
