#include "Common.h"
#include "JoystickTask.h"
#include "OledTask.h"

static Mailbox_Handle mailboxHandle;
static Mailbox_Params mailboxParams;

int main(void) {
  Board_initGeneral(CPU_FREQ);
  Board_initGPIO();
  Board_initI2C();
  Board_initSPI();

  Mailbox_Params_init(&mailboxParams);
  mailboxHandle = Mailbox_create(sizeof(int8_t) * 2, 1, &mailboxParams, NULL);

  if (mailboxHandle == NULL) {
    System_abort("Mailbox create failed\n");
  }

  JoystickTask_init(mailboxHandle);
  OledTask_init(mailboxHandle);

  BIOS_start();

  return 0;
}
