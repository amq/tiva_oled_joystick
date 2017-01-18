#pragma once

#ifdef I2CM_7
#define I2C_DESC Board_I2C0
#define GPIO_JOYSTICK_BASE_RST GPIO_PORTC_BASE
#define GPIO_JOYSTICK_PIN_RST GPIO_PIN_7
#endif

#ifdef I2CM_8
#define I2C_DESC Board_I2C1
#define GPIO_JOYSTICK_BASE_RST GPIO_PORTP_BASE
#define GPIO_JOYSTICK_PIN_RST GPIO_PIN_4
#endif

#define AS5013_ADDR 0x40
#define AS5013_CONTROL1 0x0F
#define AS5013_CONTROL2 0x2E
#define AS5013_X 0x10
#define AS5013_Y 0x11
#define AS5013_SENSITIVITY 0x2A

void JoystickTask_init(Mailbox_Handle mailboxHandle);
