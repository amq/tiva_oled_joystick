#include "Common.h"
#include "JoystickTask.h"

static void JoystickTaskFxn(UArg arg0, UArg arg1);

/*
 * @brief creates the joystick task
 *
 * @param mailboxHandle the mailbox to use in task
 */
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
    System_abort("JoystickTask create failed\n");
  }
}

/*
 * @brief polls the joystick for coordinates and posts them to mailbox
 *
 * @param arg0 the mailbox handle
 * @param arg1 not used
 */
static void JoystickTaskFxn(UArg arg0, UArg arg1) {
  I2C_Handle i2cHandle;
  I2C_Params i2cParams;
  I2C_Transaction i2cTransaction;

  uint8_t txBuffer[2] = { 0 };
  uint8_t rxBuffer[2] = { 0 };
  int8_t coordinates[2] = { 0 };

  /* hw reset */
  GPIOPinTypeGPIOOutput(GPIO_JOYSTICK_BASE_RST, GPIO_JOYSTICK_PIN_RST);
  GPIOPinWrite(GPIO_JOYSTICK_BASE_RST, GPIO_JOYSTICK_PIN_RST, GPIO_JOYSTICK_PIN_RST);
  Task_sleep(10);
  GPIOPinWrite(GPIO_JOYSTICK_BASE_RST, GPIO_JOYSTICK_PIN_RST, 0);
  Task_sleep(10);
  GPIOPinWrite(GPIO_JOYSTICK_BASE_RST, GPIO_JOYSTICK_PIN_RST, GPIO_JOYSTICK_PIN_RST);
  Task_sleep(100);

  I2C_Params_init(&i2cParams);
  i2cHandle = I2C_open(I2C_DESC, &i2cParams); /* interface addr */

  if (i2cHandle == NULL) {
    System_abort("I2C open failed\n");
  }

  i2cTransaction.slaveAddress = AS5013_ADDR; /* joystick addr */
  i2cTransaction.writeBuf = txBuffer;
  i2cTransaction.readBuf = rxBuffer;
  i2cTransaction.writeCount = 1;
  i2cTransaction.readCount = 1;

  /* soft reset */
  txBuffer[0] = AS5013_CONTROL1; /* settings register */
  (void)I2C_transfer(i2cHandle, &i2cTransaction);
  txBuffer[0] = 0x02;
  (void)I2C_transfer(i2cHandle, &i2cTransaction);

  while (1) {
    Task_sleep(10);

    txBuffer[0] = AS5013_X; /* x coordinate register */
    if (!I2C_transfer(i2cHandle, &i2cTransaction)) {
      continue; /* skip posting invalid data */
    }
    coordinates[0] = rxBuffer[0];

    txBuffer[0] = AS5013_Y; /* y coordinate register */
    if (!I2C_transfer(i2cHandle, &i2cTransaction)) {
      continue; /* skip posting invalid data */
    }
    coordinates[1] = rxBuffer[0];

    v("x: %d; y: %d\n", coordinates[0], coordinates[1]);
    (void)Mailbox_post((Mailbox_Handle)arg0, &coordinates, BIOS_NO_WAIT);
  }

  I2C_close(i2cHandle);
}
