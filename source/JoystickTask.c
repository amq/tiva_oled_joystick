#include "Common.h"
#include "JoystickTask.h"

static void JoystickTaskFxn(UArg arg0, UArg arg1);

void JoystickTask_init(Mailbox_Handle mailboxHandle) {
  Task_Handle taskHandle;
  Task_Params taskParams;
  Error_Block eb;

  Error_init(&eb);
  Task_Params_init(&taskParams);
  taskParams.stackSize = 1024;
  taskParams.priority = 10;
  taskParams.arg0 = (UArg)mailboxHandle;
  taskHandle = Task_create((Task_FuncPtr)JoystickTaskFxn, &taskParams, &eb);

  if (taskHandle == NULL) {
    System_abort("JoystickTask create failed");
  }
}

static void JoystickTaskFxn(UArg arg0, UArg arg1) {
  I2C_Handle i2cHandle;
  I2C_Params i2cParams;
  I2C_Transaction i2cTransaction;

  uint8_t txBuffer[2] = {0};
  uint8_t rxBuffer[2] = {0};

  I2C_Params_init(&i2cParams);
  i2cHandle = I2C_open(I2C_DESC, &i2cParams);

  if (i2cHandle == NULL) {
    System_abort("I2C open failed\n");
  }

  i2cTransaction.slaveAddress = AS5013_ADDR;
  i2cTransaction.writeBuf = txBuffer;
  i2cTransaction.readBuf = rxBuffer;
  i2cTransaction.writeCount = 2;
  i2cTransaction.readCount = 2;

  /* Soft reset */
  txBuffer[0] = AS5013_CONTROL1;
  txBuffer[1] = 0x02;
  (void)I2C_transfer(i2cHandle, &i2cTransaction);

  i2cTransaction.writeCount = 1;

  while (1) {
    Task_sleep(5);

    txBuffer[0] = AS5013_X;
    txBuffer[1] = AS5013_Y;
    (void)I2C_transfer(i2cHandle, &i2cTransaction);

    (void)Mailbox_post((Mailbox_Handle)arg0, rxBuffer, BIOS_NO_WAIT);
  }

  I2C_close(i2cHandle);
}
