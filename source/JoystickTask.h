#pragma once

#define JOYSTICK_BOOSTERPACK_2

#ifdef JOYSTICK_BOOSTERPACK_1
#define I2C_DESC Board_I2C0
#endif

#ifdef JOYSTICK_BOOSTERPACK_2
#define I2C_DESC Board_I2C1
#endif

#define AS5013_ADDR 0x40
#define AS5013_CONTROL1 0x0F
#define AS5013_CONTROL2 0x2E
#define AS5013_X 0x10
#define AS5013_Y 0x11
#define AS5013_SENSITIVITY 0x2A

void JoystickTask_init(Mailbox_Handle mailboxHandle);
